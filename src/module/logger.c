#include "module/logger.h"

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct logger {
    FILE *log_file;
    size_t debug_level;
    bool stderr_enable;
} logger_t;

static logger_t *p_logger = NULL;

void logger_init(const char *const filename, const size_t debug_level, const bool stderr_enable) {
    assert(filename);
    p_logger = malloc(sizeof(logger_t));
    if (!p_logger) {
        fprintf(stderr, "Failed when creating logger\n");
        abort();
    }
    assert(p_logger);
    p_logger->log_file = fopen(filename, "w");
    if (!p_logger->log_file) {
        fprintf(stderr, "Failed when open %s\n", filename);
        abort();
    };
    p_logger->debug_level = debug_level;
    assert(p_logger->debug_level <= 2);
    p_logger->stderr_enable = stderr_enable;
    return;
}

static char logger_buffer[1 << 20];

int logger_write(const log_level level, const char *const format, ...) {
    assert(p_logger);
    assert(format);

    const char *level_str = "";
    switch (level) {
    case LOG_LEVEL_ERROR:
        level_str = "ERROR";
        break;
    case LOG_LEVEL_WARNING:
        level_str = "WARNING";
        break;
    case LOG_LEVEL_INFO:
        if (p_logger->debug_level < 1)
            return 0;
        level_str = "INFO";
        break;
    case LOG_LEVEL_DEBUG:
        if (p_logger->debug_level < 2)
            return 0;
        level_str = "DEBUG";
        break;
    default:
        abort();
    }

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char *time_str = asctime(timeinfo);
    *(time_str + strlen(time_str) - 1) = '\0';

    char *ptr = logger_buffer;
    sprintf(ptr, "[%s] [%s] ", level_str, time_str);
    ptr = ptr + strlen(ptr);

    va_list args;
    va_start(args, format);
    int result = vsprintf(ptr, format, args);
    ptr = ptr + strlen(ptr);

    va_end(args);

    sprintf(ptr, "\n");
    ptr = ptr + strlen(ptr);

    fprintf(p_logger->log_file, "%s", logger_buffer);
    if (p_logger->stderr_enable)
        fprintf(stderr, "%s", logger_buffer);
    if (level == LOG_LEVEL_ERROR || level == LOG_LEVEL_WARNING || p_logger->debug_level == 2)
        fflush(p_logger->log_file);
    return result;
}

void logger_hex(const log_level level, const uint8_t *binary_string, const size_t len) {
    char *buffer = malloc(1 << 20);
    assert(buffer);
    char *p = buffer;
    *p++ = '\n';
    *p++ = '\t';
    for (size_t i = 0; i < len; ++i) {
        sprintf(p, "%02x", binary_string[i]);
        p += 2;
        if (i % 16 == 15) {
            *p++ = '\n';
            *p++ = '\t';
        } else
            *p++ = ' ';
    }
    sprintf(p, "finished.");
    logger_write(level, "%s", buffer);
    free(buffer);
    return;
}

void logger_dns_message(const log_level level, const dns_message_t *dns_message) {
    char *buffer = malloc(1 << 20);
    assert(buffer);
    char *ptr = buffer;
    sprintf(ptr, "\n\tid=0x%04" PRIx16 "\n", dns_message->header->id);
    ptr = ptr + strlen(ptr);
    sprintf(ptr, "\tqr=%d, opcode=%d, aa=%d, tc=%d, rd=%d, ra=%d, z=%d, ad=%d, cd=%d, rcode=%d \n",
            dns_message->header->flag.flags.qr,
            dns_message->header->flag.flags.opcode,
            dns_message->header->flag.flags.aa,
            dns_message->header->flag.flags.tc,
            dns_message->header->flag.flags.rd,
            dns_message->header->flag.flags.ra,
            dns_message->header->flag.flags.z,
            dns_message->header->flag.flags.ad,
            dns_message->header->flag.flags.cd,
            dns_message->header->flag.flags.rcode);
    ptr = ptr + strlen(ptr);
    sprintf(ptr, "\tqdcount=%" PRIu16 ", ancount=%" PRIu16 ", nscount=%" PRIu16 ", arcount=%" PRIu16 "\n",
            dns_message->header->qdcount,
            dns_message->header->ancount,
            dns_message->header->nscount,
            dns_message->header->arcount);
    ptr = ptr + strlen(ptr);

    forward_list_node_t *list_ptr;

    sprintf(ptr, "\tquestions:\n");
    ptr = ptr + strlen(ptr);

    list_ptr = dns_message->questions;
    for (size_t i = 0; i < dns_message->header->qdcount; ++i) {
        assert(list_ptr);
        question_t *question = (question_t *)(list_ptr->value);
        char *name = get_name_from_name_field(question->qname);
        sprintf(ptr, "\t\tqname=%s, qtype=%" PRIu16 ", qclass=%" PRIu16 "\n",
                name,
                question->qtype,
                question->qclass);
        ptr = ptr + strlen(ptr);
        list_ptr = list_ptr->next;
        free(name);
    }
    assert(list_ptr == NULL);

    sprintf(ptr, "\tanswers:\n");
    ptr = ptr + strlen(ptr);

    list_ptr = dns_message->answers;
    for (size_t i = 0; i < dns_message->header->ancount; ++i) {
        assert(list_ptr);
        resource_record_t *resource_record = list_ptr->value;
        char *name = get_name_from_name_field(resource_record->name);
        sprintf(ptr, "\t\tname=%s, type=%" PRIu16 ", class=%" PRIu16 ", ttl=%" PRIu32 ", rdlength=%" PRIu16 "\n",
                name,
                resource_record->type,
                resource_record->class,
                resource_record->ttl,
                resource_record->rd_length);
        ptr = ptr + strlen(ptr);
        list_ptr = list_ptr->next;
        free(name);
    }
    assert(list_ptr == NULL);

    logger_write(level, "%s", buffer);
    free(buffer);
    return;
}
