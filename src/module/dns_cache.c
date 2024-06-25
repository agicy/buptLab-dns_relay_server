#include "module/dns_cache.h"
#include "data_structure/list.h"
#include "data_structure/trie.h"
#include "network/dns_utility.h"

#include <time.h>

static trie_t cache_trie = NULL;
static size_t limit = -1;
static size_t item_count = 0;

void dns_cache_init(size_t item_limit) {
    limit = item_limit;
    cache_trie = trie_create();
    item_count = 0;
    return;
}

static inline void resource_record_forward_list_destroy(void *q) {
    forward_list_destroy(q, resource_record_destroy);
    return;
}

static inline void cache_refresh() {
    if (item_count >= limit) {
        trie_destroy(cache_trie, resource_record_forward_list_destroy);
        cache_trie = NULL;
        cache_trie = trie_create();
    }
    return;
}

void dns_cache_insert(const question_t *const question, const resource_record_t *const resource_record) {
    ++item_count;
    trie_node_t *tree_node_ptr = trie_insert(cache_trie, question->qname->name, question->qname->length);
    tree_node_ptr->value = forward_list_node_create(tree_node_ptr->value);
    resource_record_t *cached_resource_record = clone_resource_record((void *)resource_record);
    cached_resource_record->ttl = cached_resource_record->ttl + time(NULL);
    ((forward_list_node_t *)tree_node_ptr->value)->value = cached_resource_record;
    cache_refresh();
    return;
}

forward_list_node_t *dns_cache_query(const question_t *const question) {
    trie_node_t *tree_node_ptr = trie_find(cache_trie, question->qname->name, question->qname->length);
    if (!tree_node_ptr || !tree_node_ptr->value)
        return NULL;
    forward_list_node_t *list_ptr = tree_node_ptr->value;
    forward_list_node_t *result = NULL;
    while (list_ptr) {
        resource_record_t *resource_record = list_ptr->value;
        if (resource_record->type == question->qtype && resource_record->class == question->qclass && resource_record->ttl > time(NULL)) {
            result = forward_list_node_create(result);
            resource_record_t *delivered_resource_record = clone_resource_record(resource_record);
            delivered_resource_record->ttl = delivered_resource_record->ttl - time(NULL);
            result->value = delivered_resource_record;
        }
        list_ptr = list_ptr->next;
    }
    return result;
}
