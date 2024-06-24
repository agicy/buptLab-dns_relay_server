#include "data_structure/trie.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static inline trie_node_t *trie_node_create(trie_node_t *fa) {
    trie_node_t *p = malloc(sizeof(trie_node_t));
    assert(p);
    p->count = 0;
    p->fa = fa;
    memset(p->ch, 0, sizeof(trie_node_t *) * TRIE_RADIX);
    p->value = NULL;
    return p;
}

static inline void trie_node_destroy(trie_node_t *p) {
    assert(p);
    p->count = 0;
    p->fa = NULL;
    for (size_t i = 0; i < TRIE_RADIX; ++i)
        p->ch[i] = NULL;
    p->value = NULL;
    free(p);
    return;
}

trie_t trie_create(void) {
    trie_t p = trie_node_create(NULL);
    return p;
}

void trie_destroy(trie_t p, void (*value_destroy)(void *)) {
    assert(p);
    for (size_t i = 0; i < TRIE_RADIX; ++i)
        if (p->ch[i]) {
            trie_destroy(p->ch[i], value_destroy);
            p->ch[i] = NULL;
        }
    if (value_destroy && p->value)
        (*value_destroy)(p->value);
    p->value = NULL;
    trie_node_destroy(p);
    p = NULL;
    return;
}

trie_node_t *trie_find(trie_t rt, const trie_radix_t *binary_string, size_t len) {
    assert(rt);
    trie_node_t *p = rt;
    for (size_t i = 0; i < len; ++i)
        if (p->ch[binary_string[i]])
            p = p->ch[binary_string[i]];
        else
            return NULL;
    return p;
}

trie_node_t *trie_insert(trie_t rt, const trie_radix_t *binary_string, size_t len) {
    assert(rt);
    trie_node_t *p = rt;
    for (size_t i = 0; i < len; ++i) {
        if (!p->ch[binary_string[i]])
            p->ch[binary_string[i]] = trie_node_create(p);
        p = p->ch[binary_string[i]];
    }
    ++p->count;
    return p;
}
