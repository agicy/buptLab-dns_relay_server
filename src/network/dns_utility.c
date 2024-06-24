#include "network/dns_utility.h"
#include "network/ipv4_utility.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

name_field_t *name_field_create(const char *const name, size_t length) {
    name_field_t *p = malloc(sizeof(name_field_t));
    assert(p);
    p->length = length + 2;
    p->name = malloc(p->length);
    assert(p->name);
    uint8_t *ptr = p->name;
    for (size_t l = 0, r; l < length; l = r + 1) {
        r = l;
        while (r < length && name[r] != '.')
            ++r;
        size_t count = r - l;
        *ptr = count;
        memcpy(ptr + 1, name + l, count);
        ptr += (count + 1);
    }
    *ptr = 0;
    assert(ptr + 1 == p->length + p->name);
    return p;
}

char *get_name_from_name_field(const name_field_t *const name_field) {
    assert(name_field->length > 1);
    char *result = malloc(name_field->length - 1);
    char *base = result;
    assert(result);
    const uint8_t *ptr = name_field->name;
    while (*ptr) {
        memcpy(base, ptr + 1, *ptr);
        base += *ptr;
        *base++ = '.';
        ptr += (*ptr + 1);
    }
    *--base = '\0';
    return result;
}

static size_t name_length_default = 512;

dns_message_t *parse_dns_message(const char *const base) {
    const char *ptr = base;
    dns_message_t *p = malloc(sizeof(dns_message_t));
    assert(p);

    p->header = malloc(sizeof(dns_header_t));
    assert(p->header);
    memcpy(p->header, ptr, sizeof(dns_header_t));
    p->header->id = ntohs(p->header->id);
    p->header->flag.value = ntohs(p->header->flag.value);
    p->header->qdcount = ntohs(p->header->qdcount);
    p->header->ancount = ntohs(p->header->ancount);
    p->header->nscount = ntohs(p->header->nscount);
    p->header->arcount = ntohs(p->header->arcount);

    ptr += sizeof(dns_header_t);

    p->questions = NULL;
    for (size_t i = 0; i < p->header->qdcount; ++i) {
        const char *x = ptr;
        bool compressed = false;

        name_field_t *name_field = malloc(sizeof(name_field_t));
        name_field->name = malloc(name_length_default);
        uint8_t *name_ptr = name_field->name;
        name_field->length = 0;
        while (*x) {
            if (((*x) & 0xc0) == 0xc0) {
                if (!compressed)
                    ptr = x + 2;
                compressed = true;
                uint8_t first = (*x) & 0x3f;
                uint8_t second = *(x + 1);
                uint16_t offset = (((uint16_t)first) << 8) | second;
                x = base + offset;
            } else {
                name_field->length += (*x + 1);
                memcpy(name_ptr, x, *x + 1);
                name_ptr += (*x + 1);
                x += (*x + 1);
            }
        }
        *name_ptr = 0;
        ++name_field->length;
        if (!compressed)
            ptr = x + 1;

        question_t *question = malloc(sizeof(question_t));
        assert(question);
        question->qname = name_field;
        memcpy(&question->qtype, ptr, sizeof(question->qtype));
        ptr += sizeof(question->qtype);
        question->qtype = ntohs(question->qtype);

        memcpy(&question->qclass, ptr, sizeof(question->qclass));
        ptr += sizeof(question->qclass);
        question->qclass = ntohs(question->qclass);

        p->questions = forward_list_node_create(p->questions);
        p->questions->value = question;
    }

    p->answers = NULL;
    for (size_t i = 0; i < p->header->ancount; ++i) {
        const char *x = ptr;
        bool compressed = false;

        name_field_t *name_field = malloc(sizeof(name_field_t));
        name_field->name = malloc(name_length_default);
        uint8_t *name_ptr = name_field->name;
        name_field->length = 0;
        while (*x) {
            if (((*x) & 0xc0) == 0xc0) {
                if (!compressed)
                    ptr = x + 2;
                compressed = true;
                uint8_t first = (*x) & 0x3f;
                uint8_t second = *(x + 1);
                uint16_t offset = (((uint16_t)first) << 8) | second;
                x = base + offset;
            } else {
                name_field->length += (*x + 1);
                memcpy(name_ptr, x, *x + 1);
                name_ptr += (*x + 1);
                x += (*x + 1);
            }
        }
        *name_ptr = 0;
        ++name_field->length;
        if (!compressed)
            ptr = x + 1;

        resource_record_t *answer = malloc(sizeof(resource_record_t));
        assert(answer);
        answer->name = name_field;

        memcpy(&answer->type, ptr, sizeof(answer->type));
        ptr += sizeof(answer->type);
        answer->type = ntohs(answer->type);

        memcpy(&answer->class, ptr, sizeof(answer->class));
        ptr += sizeof(answer->class);
        answer->class = ntohs(answer->class);

        memcpy(&answer->ttl, ptr, sizeof(answer->ttl));
        ptr += sizeof(answer->ttl);
        answer->ttl = ntohl(answer->ttl);

        memcpy(&answer->rd_length, ptr, sizeof(answer->rd_length));
        ptr += sizeof(answer->rd_length);
        answer->rd_length = ntohs(answer->rd_length);

        answer->rdata = malloc(answer->rd_length);
        memcpy(answer->rdata, ptr, answer->rd_length);
        ptr += answer->rd_length;

        p->answers = forward_list_node_create(p->answers);
        p->answers->value = answer;
    }

    p->authorities = NULL;
    for (size_t i = 0; i < p->header->nscount; ++i) {
        const char *x = ptr;
        bool compressed = false;

        name_field_t *name_field = malloc(sizeof(name_field_t));
        name_field->name = malloc(name_length_default);
        uint8_t *name_ptr = name_field->name;
        name_field->length = 0;
        while (*x) {
            if (((*x) & 0xc0) == 0xc0) {
                if (!compressed)
                    ptr = x + 2;
                compressed = true;
                uint8_t first = (*x) & 0x3f;
                uint8_t second = *(x + 1);
                uint16_t offset = (((uint16_t)first) << 8) | second;
                x = base + offset;
            } else {
                name_field->length += (*x + 1);
                memcpy(name_ptr, x, *x + 1);
                name_ptr += (*x + 1);
                x += (*x + 1);
            }
        }
        *name_ptr = 0;
        ++name_field->length;
        if (!compressed)
            ptr = x + 1;

        resource_record_t *authority = malloc(sizeof(resource_record_t));
        assert(authority);
        authority->name = name_field;

        memcpy(&authority->type, ptr, sizeof(authority->type));
        ptr += sizeof(authority->type);
        authority->type = ntohs(authority->type);

        memcpy(&authority->class, ptr, sizeof(authority->class));
        ptr += sizeof(authority->class);
        authority->class = ntohs(authority->class);

        memcpy(&authority->ttl, ptr, sizeof(authority->ttl));
        ptr += sizeof(authority->ttl);
        authority->ttl = ntohl(authority->ttl);

        memcpy(&authority->rd_length, ptr, sizeof(authority->rd_length));
        ptr += sizeof(authority->rd_length);
        authority->rd_length = ntohs(authority->rd_length);

        authority->rdata = malloc(authority->rd_length);
        memcpy(authority->rdata, ptr, authority->rd_length);
        ptr += authority->rd_length;

        p->authorities = forward_list_node_create(p->authorities);
        p->authorities->value = authority;
    }

    p->additionals = NULL;
    for (size_t i = 0; i < p->header->arcount; ++i) {
        const char *x = ptr;
        bool compressed = false;

        name_field_t *name_field = malloc(sizeof(name_field_t));
        name_field->name = malloc(name_length_default);
        uint8_t *name_ptr = name_field->name;
        name_field->length = 0;
        while (*x) {
            if (((*x) & 0xc0) == 0xc0) {
                if (!compressed)
                    ptr = x + 2;
                compressed = true;
                uint8_t first = (*x) & 0x3f;
                uint8_t second = *(x + 1);
                uint16_t offset = (((uint16_t)first) << 8) | second;
                x = base + offset;
            } else {
                name_field->length += (*x + 1);
                memcpy(name_ptr, x, *x + 1);
                name_ptr += (*x + 1);
                x += (*x + 1);
            }
        }
        *name_ptr = 0;
        ++name_field->length;
        if (!compressed)
            ptr = x + 1;

        resource_record_t *additional = malloc(sizeof(resource_record_t));
        assert(additional);
        additional->name = name_field;

        memcpy(&additional->type, ptr, sizeof(additional->type));
        ptr += sizeof(additional->type);
        additional->type = ntohs(additional->type);

        memcpy(&additional->class, ptr, sizeof(additional->class));
        ptr += sizeof(additional->class);
        additional->class = ntohs(additional->class);

        memcpy(&additional->ttl, ptr, sizeof(additional->ttl));
        ptr += sizeof(additional->ttl);
        additional->ttl = ntohl(additional->ttl);

        memcpy(&additional->rd_length, ptr, sizeof(additional->rd_length));
        ptr += sizeof(additional->rd_length);
        additional->rd_length = ntohs(additional->rd_length);

        additional->rdata = malloc(additional->rd_length);
        memcpy(additional->rdata, ptr, additional->rd_length);
        ptr += additional->rd_length;

        p->additionals = forward_list_node_create(p->additionals);
        p->additionals->value = additional;
    }

    return p;
}

static inline uint8_t *appends(uint8_t *ptr, uint16_t value) {
    value = htons(value);
    memcpy(ptr, &value, sizeof(uint16_t));
    ptr += sizeof(uint16_t);
    return ptr;
}

static inline uint8_t *appendl(uint8_t *ptr, uint32_t value) {
    value = htonl(value);
    memcpy(ptr, &value, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    return ptr;
}

uint8_t *convert_dns_message_to_stream(const dns_message_t *const dns_message, uint8_t *const buffer) {
    uint8_t *ptr = buffer;

    ptr = appends(ptr, dns_message->header->id);
    ptr = appends(ptr, dns_message->header->flag.value);
    ptr = appends(ptr, dns_message->header->qdcount);
    ptr = appends(ptr, dns_message->header->ancount);
    ptr = appends(ptr, dns_message->header->nscount);
    ptr = appends(ptr, dns_message->header->arcount);

    forward_list_node_t *list_ptr;

    list_ptr = dns_message->questions;
    for (size_t i = 0; i < dns_message->header->qdcount; ++i) {
        question_t *question = list_ptr->value;

        memcpy(ptr, question->qname->name, question->qname->length);
        ptr += question->qname->length;
        ptr = appends(ptr, question->qtype);
        ptr = appends(ptr, question->qclass);

        list_ptr = list_ptr->next;
    }
    assert(list_ptr == NULL);

    list_ptr = dns_message->answers;
    for (size_t i = 0; i < dns_message->header->ancount; ++i) {
        resource_record_t *resource_record = list_ptr->value;

        memcpy(ptr, resource_record->name->name, resource_record->name->length);
        ptr += resource_record->name->length;
        ptr = appends(ptr, resource_record->type);
        ptr = appends(ptr, resource_record->class);
        ptr = appendl(ptr, resource_record->ttl);
        ptr = appends(ptr, resource_record->rd_length);
        memcpy(ptr, resource_record->rdata, resource_record->rd_length);
        ptr += resource_record->rd_length;

        list_ptr = list_ptr->next;
    }
    assert(list_ptr == NULL);

    list_ptr = dns_message->authorities;
    for (size_t i = 0; i < dns_message->header->nscount; ++i) {
        resource_record_t *resource_record = list_ptr->value;

        memcpy(ptr, resource_record->name->name, resource_record->name->length);
        ptr += resource_record->name->length;
        ptr = appends(ptr, resource_record->type);
        ptr = appends(ptr, resource_record->class);
        ptr = appendl(ptr, resource_record->ttl);
        ptr = appends(ptr, resource_record->rd_length);
        memcpy(ptr, resource_record->rdata, resource_record->rd_length);
        ptr += resource_record->rd_length;

        list_ptr = list_ptr->next;
    }
    assert(list_ptr == NULL);

    list_ptr = dns_message->additionals;
    for (size_t i = 0; i < dns_message->header->arcount; ++i) {
        resource_record_t *resource_record = list_ptr->value;

        memcpy(ptr, resource_record->name->name, resource_record->name->length);
        ptr += resource_record->name->length;
        ptr = appends(ptr, resource_record->type);
        ptr = appends(ptr, resource_record->class);
        ptr = appendl(ptr, resource_record->ttl);
        ptr = appends(ptr, resource_record->rd_length);
        memcpy(ptr, resource_record->rdata, resource_record->rd_length);
        ptr += resource_record->rd_length;

        list_ptr = list_ptr->next;
    }
    assert(list_ptr == NULL);

    return ptr;
}

dns_header_t *clone_dns_header(const dns_header_t *const dns_header) {
    dns_header_t *new_header = malloc(sizeof(dns_header_t));
    assert(new_header);
    memcpy(new_header, dns_header, sizeof(dns_header_t));
    return new_header;
}

name_field_t *clone_name_field(const name_field_t *const name_field) {
    name_field_t *new_name_field = malloc(sizeof(name_field_t));
    assert(new_name_field);
    new_name_field->length = name_field->length;
    new_name_field->name = malloc(new_name_field->length);
    assert(new_name_field->name);
    memcpy(new_name_field->name, name_field->name, new_name_field->length);
    return new_name_field;
}

void *clone_question(void *q) {
    question_t *question = q;
    question_t *new_question = malloc(sizeof(question_t));
    assert(new_question);
    new_question->qname = clone_name_field(question->qname);
    new_question->qtype = question->qtype;
    new_question->qclass = question->qclass;
    return new_question;
}

void *clone_resource_record(void *q) {
    resource_record_t *resource_record = q;
    resource_record_t *new_resource_record = malloc(sizeof(resource_record_t));
    assert(new_resource_record);
    new_resource_record->name = clone_name_field(resource_record->name);
    new_resource_record->type = resource_record->type;
    new_resource_record->class = resource_record->class;
    new_resource_record->ttl = resource_record->ttl;
    new_resource_record->rd_length = resource_record->rd_length;
    new_resource_record->rdata = malloc(new_resource_record->rd_length);
    assert(new_resource_record->rdata);
    memcpy(new_resource_record->rdata, resource_record->rdata, new_resource_record->rd_length);
    return new_resource_record;
}

dns_message_t *clone_dns_message(const dns_message_t *const dns_message) {
    dns_message_t *new_message = malloc(sizeof(dns_message_t));
    assert(new_message);
    new_message->header = clone_dns_header(dns_message->header);
    new_message->questions = forward_list_clone(dns_message->questions, clone_question);
    new_message->answers = forward_list_clone(dns_message->answers, clone_resource_record);
    new_message->authorities = forward_list_clone(dns_message->authorities, clone_resource_record);
    new_message->additionals = forward_list_clone(dns_message->additionals, clone_resource_record);
    return new_message;
}

void name_field_destroy(name_field_t *p) {
    assert(p);
    assert(p->name);
    free(p->name);
    p->name = NULL;
    p->length = 0;
    free(p);
    p = NULL;
    return;
}

void dns_header_destroy(dns_header_t *q) {
    dns_header_t *p = q;
    assert(p);
    free(p);
    p = NULL;
    return;
}

void question_destroy(void *q) {
    question_t *p = q;
    assert(p);
    name_field_destroy(p->qname);
    p->qname = NULL;
    p->qtype = 0;
    p->qclass = 0;
    free(p);
    p = NULL;
    return;
}

void resource_record_destroy(void *q) {
    resource_record_t *p = q;
    assert(p);
    name_field_destroy(p->name);
    p->name = NULL;
    p->type = 0;
    p->class = 0;
    p->ttl = 0;
    p->rd_length = 0;
    assert(p->rdata);
    free(p->rdata);
    p->rdata = NULL;
    free(p);
    p = NULL;
    return;
}

void dns_message_destroy(dns_message_t *p) {
    assert(p);
    assert(p->header);
    dns_header_destroy(p->header);
    p->header = NULL;
    if (p->questions)
        forward_list_destroy(p->questions, question_destroy);
    p->questions = NULL;
    if (p->answers)
        forward_list_destroy(p->answers, resource_record_destroy);
    p->answers = NULL;
    if (p->authorities)
        forward_list_destroy(p->authorities, resource_record_destroy);
    p->authorities = NULL;
    if (p->additionals)
        forward_list_destroy(p->additionals, resource_record_destroy);
    p->additionals = NULL;
    free(p);
    p = NULL;
    return;
}
