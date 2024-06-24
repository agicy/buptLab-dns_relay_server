#include "network/ipv4_utility.h"
#include <arpa/inet.h>
#include <netinet/in.h>

in_addr_t get_ipv4_from_string(const char *const address) {
    return inet_addr(address);
}
