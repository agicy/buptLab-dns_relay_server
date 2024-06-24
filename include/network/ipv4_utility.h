/**
 * @file ipv4_utility.h
 * @brief This file provides a function for converting a string to an IPv4 address.
 */

#pragma once
#ifndef IPV4_UTILITY_H
#define IPV4_UTILITY_H

#include <netinet/in.h>

/**
 * @brief Get an IPv4 address from a string.
 *
 * This function converts a string to an IPv4 address.
 *
 * @param address The string containing the IPv4 address.
 * @return The IPv4 address.
 */
in_addr_t get_ipv4_from_string(const char *const address);

#endif
