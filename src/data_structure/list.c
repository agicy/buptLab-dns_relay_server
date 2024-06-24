#include "data_structure/list.h"

#include <assert.h>
#include <stdlib.h>

list_node_t *list_node_create(void) {
    list_node_t *p = malloc(sizeof(list_node_t));
    assert(p);
    p->value = NULL;
    p->prev = NULL;
    p->next = NULL;
    return p;
}

void list_node_destroy(list_node_t *p) {
    p->value = NULL;
    p->prev = NULL;
    p->next = NULL;
    free(p);
    p = NULL;
    return;
}
