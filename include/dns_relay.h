/**
 * @file dns_relay.h
 * @brief Header file for DNS relay.
 */

#pragma once
#ifndef DNS_RELAY_H
#define DNS_RELAY_H

/**
 * @def BUF_SIZE
 * @brief Buffer size for DNS relay.
 *
 * This macro defines the buffer size to be used in the DNS relay.
 * The size is defined as 2 to the power of 20.
 */
#define BUF_SIZE (2 << 20)

/**
 * @def DNS_PORT
 * @brief Port number for DNS.
 *
 * This macro defines the port number to be used for DNS.
 * The standard port for DNS is 53.
 */
#define DNS_PORT 53

/**
 * @var int sockfd
 * @brief Socket file descriptor.
 *
 * This variable holds the socket file descriptor.
 * It is initialized to -1, indicating that the socket is not yet open.
 */
int sockfd = -1;

#endif
