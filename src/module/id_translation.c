#include "module/id_translation.h"

#include <string.h>

typedef struct storage {
    uint16_t original_id;
    struct sockaddr_in address;
} storage_t;

static uint16_t all_id = 0;
static storage_t array[65536];

uint16_t nid_create(void) {
    uint16_t result = all_id++;
    return result;
}

void set_original_id(uint16_t nid, uint16_t original_id) {
    array[nid].original_id = original_id;
}

void set_client_address(uint16_t nid, const struct sockaddr_in *const address) {
    array[nid].address = *address;
    return;
}

uint16_t get_original_id(uint16_t nid) {
    return array[nid].original_id;
}

struct sockaddr_in *get_client_address(uint16_t nid) {
    return &array[nid].address;
}

void nid_release(uint16_t nid) {
    memset(&array[nid], 0, sizeof(storage_t));
    return;
}
