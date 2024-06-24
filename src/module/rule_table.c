#include "module/rule_table.h"
#include "data_structure/forward_list.h"
#include "data_structure/trie.h"
#include "module/logger.h"
#include "network/dns_utility.h"
#include "network/ipv4_utility.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const size_t rule_table_buffer_size = 1 << 20;
static trie_t banned_name_trie = NULL;
static trie_t configured_name_trie = NULL;

static inline void handle_banned_name(const char *const name) {
    assert(banned_name_trie);
    trie_insert(banned_name_trie, (trie_radix_t *)name, strlen(name));
    return;
}

static inline void handle_configured_name(const char *const ipv4, const char *const name) {
    in_addr_t address = get_ipv4_from_string(ipv4);
    name_field_t *name_field = name_field_create(name, strlen(name));
    resource_record_t *record = malloc(sizeof(resource_record_t));
    record->name = name_field;
    record->type = 1;  // A
    record->class = 1; // IN
    record->ttl = 0;
    record->rd_length = sizeof(in_addr_t);
    record->rdata = malloc(record->rd_length);
    memcpy(record->rdata, &address, sizeof(in_addr_t));

    trie_node_t *p = trie_insert(configured_name_trie, (trie_radix_t *)name, strlen(name));
    p->value = forward_list_node_create(p->value);
    ((forward_list_node_t *)p->value)->value = record;
    return;
}

static inline void handle(const char *const ip, const char *const name) {
    if (strcmp(ip, banned_ip) == 0)
        handle_banned_name(name);
    else
        handle_configured_name(ip, name);
    return;
}

void load_rule_table(const char *const filename) {
    banned_name_trie = trie_create();
    configured_name_trie = trie_create();
    FILE *file = fopen(filename, "r");
    assert(file);
    char *ip_buffer = malloc(sizeof(char) * rule_table_buffer_size);
    assert(ip_buffer);
    char *name_buffer = malloc(sizeof(char) * rule_table_buffer_size);
    assert(name_buffer);
    while (fscanf(file, "%s %s", ip_buffer, name_buffer) != EOF)
        handle(ip_buffer, name_buffer);
    fclose(file);
    file = NULL;
    free(ip_buffer);
    ip_buffer = NULL;
    free(name_buffer);
    name_buffer = NULL;
    return;
}

bool is_banned(const char *const name) {
    assert(banned_name_trie);
    trie_node_t *p = trie_find(banned_name_trie, (trie_radix_t *)name, strlen(name));
    return p != NULL && p->count;
}

forward_list_t get_configured(const char *const name) {
    assert(configured_name_trie);
    trie_node_t *p = trie_find(configured_name_trie, (trie_radix_t *)name, strlen(name));
    if (p)
        return p->value;
    else
        return NULL;
}
