#include "data_structure/forward_list.h"

#include <assert.h>
#include <stdlib.h>

forward_list_node_t *forward_list_node_create(forward_list_node_t *next) {
    forward_list_node_t *p = malloc(sizeof(forward_list_node_t));
    assert(p);
    p->value = NULL;
    p->next = next;
    return p;
}

void forward_list_node_destroy(forward_list_node_t *p, void (*value_destroy)(void *)) {
    if (value_destroy && p->value)
        (*value_destroy)(p->value);
    p->value = NULL;
    p->next = NULL;
    free(p);
    return;
}

forward_list_t forward_list_create() {
    forward_list_node_t *p = forward_list_node_create(NULL);
    assert(p);
    return p;
}

void forward_list_destroy(forward_list_t p, void (*value_destroy)(void *)) {
    assert(p);
    if (p->next)
        forward_list_destroy(p->next, value_destroy);
    forward_list_node_destroy(p, value_destroy);
    p = NULL;
    return;
}

forward_list_t forward_list_delete(forward_list_t p, forward_list_node_t *q, void (*value_destroy)(void *)) {
    if (p == q) {
        forward_list_t res = p->next;
        forward_list_node_destroy(q, value_destroy);
        return res;
    }
    forward_list_node_t *ptr = p;
    while (assert(ptr), ptr->next != q)
        ptr = ptr->next;
    assert(ptr->next == q);
    ptr->next = ptr->next->next;
    forward_list_node_destroy(q, value_destroy);
    return p;
}

forward_list_t forward_list_clone(forward_list_t p, void *(*value_clone)(void *)) {
    if (!p)
        return NULL;
    else {
        forward_list_node_t *q = forward_list_node_create(forward_list_clone(p->next, value_clone));
        q->value = value_clone(p->value);
        return q;
    }
}
