#include "dns_relay.h"
#include "module/cmd_interpreter.h"
#include "module/dns_cache.h"
#include "module/id_translation.h"
#include "module/logger.h"
#include "module/rule_table.h"
#include "network/dns_utility.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

static inline void put_message(const dns_message_t *dns_message, const struct sockaddr_in *const address) {
    static const size_t buffer_size = 1 << 20;
    static uint8_t *buffer = NULL;
    if (!buffer)
        buffer = malloc(buffer_size);
    uint8_t *end = convert_dns_message_to_stream(dns_message, buffer);
    size_t length = end - buffer;
    sendto(sockfd, buffer, length, 0, (const struct sockaddr *)address, sizeof(*address));
    return;
}

static inline void send_nx(const dns_message_t *dns_message, const struct sockaddr_in *const client_address) {
    dns_message_t *send_message = clone_dns_message(dns_message);
    send_message->header->flag.flags.qr = 1;
    send_message->header->flag.flags.rcode = 3;
    put_message(send_message, client_address);
    dns_message_destroy(send_message);
    return;
}

static inline void send_configured_response(const dns_message_t *dns_message, forward_list_t result, const struct sockaddr_in *const client_address) {
    dns_message_t *send_message = clone_dns_message(dns_message);
    send_message->header->flag.flags.qr = 1;
    send_message->header->flag.flags.rcode = 0;
    send_message->answers = result;
    send_message->header->ancount = 0;
    forward_list_node_t *ptr = result;
    while (ptr) {
        ++send_message->header->ancount;
        ptr = ptr->next;
    }
    put_message(send_message, client_address);
    dns_message_destroy(send_message);
    return;
}

static inline void send_cached_response(const dns_message_t *dns_message, forward_list_t result, const struct sockaddr_in *const client_address) {
    dns_message_t *send_message = clone_dns_message(dns_message);
    send_message->header->flag.flags.qr = 1;
    send_message->header->flag.flags.rcode = 0;
    send_message->answers = result;
    send_message->header->ancount = 0;
    forward_list_node_t *ptr = result;
    while (ptr) {
        ++send_message->header->ancount;
        ptr = ptr->next;
    }
    put_message(send_message, client_address);
    dns_message_destroy(send_message);
    return;
}

static inline void send_relay_request(const dns_message_t *dns_message, uint16_t nid, const struct sockaddr_in *const dns_server_address) {
    dns_message_t *send_message = clone_dns_message(dns_message);
    send_message->header->id = nid;
    put_message(send_message, dns_server_address);
    dns_message_destroy(send_message);
    return;
}

static inline void send_relay_response(const dns_message_t *dns_message, const uint16_t original_id, const struct sockaddr_in *const client_address) {
    dns_message_t *send_message = clone_dns_message(dns_message);
    send_message->header->id = original_id;
    put_message(send_message, client_address);
    dns_message_destroy(send_message);
}

static inline void handle_query(const dns_message_t *dns_message, const struct sockaddr_in *const dns_server_address, const struct sockaddr_in *const client_addr) {
    assert(dns_message->header->flag.flags.opcode == 0);
    assert(dns_message->header->flag.flags.tc == 0);
    assert(dns_message->header->flag.flags.z == 0);
    assert(dns_message->header->qdcount == 1);

    forward_list_node_t *questions;
    bool banned = false;
    questions = dns_message->questions;
    for (size_t i = 0; i < dns_message->header->qdcount; ++i) {
        assert(questions);
        question_t *question = questions->value;
        char *name = get_name_from_name_field(question->qname);
        if (is_banned(name))
            banned = true;
        free(name);
        questions = questions->next;
    }
    assert(questions == NULL);
    if (banned) {
        logger_write(LOG_LEVEL_INFO, "Banned Query.");
        send_nx(dns_message, client_addr);
        return;
    }

    bool configured = true;
    questions = dns_message->questions;
    forward_list_node_t *configured_result = NULL;
    for (size_t i = 0; i < dns_message->header->qdcount; ++i) {
        assert(questions);
        question_t *question = questions->value;
        assert(question->qclass == 1);
        char *name = get_name_from_name_field(question->qname);
        forward_list_node_t *ptr = get_configured(name);
        bool found = false;
        while (ptr) {
            resource_record_t *resource_record = ptr->value;
            if (resource_record->type == question->qtype && resource_record->class == question->qclass) {
                found = true;
                configured_result = forward_list_node_create(configured_result);
                configured_result->value = clone_resource_record(resource_record);
            }
            ptr = ptr->next;
        }
        assert(ptr == NULL);
        if (!found)
            configured = false;
        free(name);
        questions = questions->next;
    }
    assert(questions == NULL);
    if (configured) {
        logger_write(LOG_LEVEL_INFO, "Configured Query.");
        send_configured_response(dns_message, configured_result, client_addr);
        return;
    } else if (configured_result)
        forward_list_destroy(configured_result, resource_record_destroy);

    bool cached = true;
    questions = dns_message->questions;
    forward_list_node_t *cached_result = NULL;
    for (size_t i = 0; i < dns_message->header->qdcount; ++i) {
        assert(questions);
        question_t *question = questions->value;
        assert(question->qclass == 1);
        bool found = false;
        cached_result = dns_cache_query(question);
        if (cached_result)
            found = true;
        if (!found)
            cached = false;
        questions = questions->next;
    }
    assert(questions == NULL);
    if (cached) {
        logger_write(LOG_LEVEL_INFO, "Cached Query.");
        send_cached_response(dns_message, cached_result, client_addr);
        return;
    } else if (cached_result)
        forward_list_destroy(cached_result, resource_record_destroy);

    logger_write(LOG_LEVEL_INFO, "Relay Query.");

    uint16_t nid = nid_create();
    set_client_address(nid, client_addr);
    set_original_id(nid, dns_message->header->id);
    send_relay_request(dns_message, nid, dns_server_address);

    return;
}

static inline void handle_response(const dns_message_t *dns_message) {
    assert(dns_message->header->qdcount == 1);
    uint16_t nid = dns_message->header->id;
    struct sockaddr_in *client_addr = get_client_address(nid);
    uint16_t original_id = get_original_id(nid);

    if (dns_message->header->flag.flags.rcode == 0) {
        forward_list_node_t *p = dns_message->answers;
        while (p) {
            dns_cache_insert(dns_message->questions->value, p->value);
            p = p->next;
        }
    }
    send_relay_response(dns_message, original_id, client_addr);
    nid_release(nid);
    return;
}

static inline void distribute_frame(const dns_message_t *dns_message, const struct sockaddr_in *const dns_server_address, const struct sockaddr_in *const client_addr) {
    if (dns_message->header->flag.flags.qr == 0) {
        // query
        handle_query(dns_message, dns_server_address, client_addr);

    } else {
        // response
        handle_response(dns_message);
    }
    return;
}
char buf[BUF_SIZE];

int main(int argc, char *argv[]) {
    cmd_opt_t options = get_options(argc, argv);
    logger_init(options.log_file_name, options.debug_level, options.stderr_enable);
    logger_write(LOG_LEVEL_INFO,
                 "\nOptions:\n\t--debug = %zu,\n\t--cache-size = %zu item,\n\t--listen-port = %" PRIu16 ",\n\t--hosts-file = %s,\n\t--dns-server = %s,\n\t--log-file = %s,\n\t--stderr-enable = %d.",
                 options.debug_level,
                 options.cache_size,
                 options.listen_port,
                 options.hosts_file_name,
                 options.isp_dns_server_ip,
                 options.log_file_name,
                 options.stderr_enable);
    load_rule_table(options.hosts_file_name);

    dns_cache_init(options.cache_size);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        logger_write(LOG_LEVEL_ERROR, "Socket creation failed!");
        abort();
    }
    logger_write(LOG_LEVEL_INFO, "Socket created successfully.");

    struct sockaddr_in listen_address;
    memset(&listen_address, 0, sizeof(listen_address));
    listen_address.sin_family = AF_INET;
    listen_address.sin_port = htons(options.listen_port);
    listen_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&listen_address, sizeof(listen_address)) < 0) {
        logger_write(LOG_LEVEL_ERROR, "Socket bind port %d failed!", DNS_PORT);
        abort();
    }
    logger_write(LOG_LEVEL_INFO, "Socket bind %d successfully.", DNS_PORT);

    struct sockaddr_in dns_server_address;
    memset(&dns_server_address, 0, sizeof(dns_server_address));
    dns_server_address.sin_family = AF_INET;
    dns_server_address.sin_port = htons(DNS_PORT);
    dns_server_address.sin_addr.s_addr = inet_addr(options.isp_dns_server_ip);

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    while (1) {
        ssize_t recv_len = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (recv_len < 0) {
            assert(recv_len == -1);
            if (errno != EINTR)
                logger_write(LOG_LEVEL_WARNING, "Failed when receiving!");
            continue;
        }
        logger_write(LOG_LEVEL_INFO, "Received %zd byte(s) from client %s:%d.", recv_len, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        logger_hex(LOG_LEVEL_DEBUG, (uint8_t *)buf, recv_len);
        dns_message_t *message = parse_dns_message(buf);

        logger_dns_message(LOG_LEVEL_DEBUG, message);
        distribute_frame(message, &dns_server_address, &client_addr);

        dns_message_destroy(message);
        message = NULL;
    }

    return 0;
}
