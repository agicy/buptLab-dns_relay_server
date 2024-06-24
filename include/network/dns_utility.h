/**
 * @file dns_utility.h
 * @brief This file provides functions for DNS message parsing and manipulation.
 */

#pragma once
#ifndef DNS_UTILITY_H
#define DNS_UTILITY_H

#include "data_structure/forward_list.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Structure representing a DNS header.
 */
typedef struct dns_header {
    uint16_t id; /**< The ID of the DNS message. */
    union flag {
        uint16_t value; /**< The value of the flags. */
        struct flags {
            uint16_t rcode : 4;  /**< The response code. */
            uint16_t cd : 1;     /**< The checking disabled flag. */
            uint16_t ad : 1;     /**< The authenticated data flag. */
            uint16_t z : 1;      /**< The reserved for future use flag. */
            uint16_t ra : 1;     /**< The recursion available flag. */
            uint16_t rd : 1;     /**< The recursion desired flag. */
            uint16_t tc : 1;     /**< The truncation flag. */
            uint16_t aa : 1;     /**< The authoritative answer flag. */
            uint16_t opcode : 4; /**< The operation code. */
            uint16_t qr : 1;     /**< The query/response flag. */
        } flags;                 /**< The flags of the DNS message. */
    } flag;
    uint16_t qdcount; /**< The number of entries in the question section. */
    uint16_t ancount; /**< The number of resource records in the answer section. */
    uint16_t nscount; /**< The number of name server resource records in the authority records section. */
    uint16_t arcount; /**< The number of resource records in the additional records section. */
} dns_header_t;

/**
 * @brief Structure representing a name field in a DNS message.
 */
typedef struct name_field {
    size_t length; /**< The length of the name. */
    uint8_t *name; /**< The name. */
} name_field_t;

/**
 * @brief Structure representing a question in a DNS message.
 */
typedef struct question {
    name_field_t *qname; /**< The domain name. */
    uint16_t qtype;      /**< The type of the query. */
    uint16_t qclass;     /**< The class of the query. */
} question_t;

/**
 * @brief Structure representing a resource record in a DNS message.
 */
typedef struct resource_record {
    name_field_t *name; /**< The domain name. */
    uint16_t type;      /**< The type of the data. */
    uint16_t class;     /**< The class of the data. */
    uint32_t ttl;       /**< The time to live. */
    uint16_t rd_length; /**< The length of rdata. */
    uint8_t *rdata;     /**< The resource data. */
} resource_record_t;

/**
 * @brief Structure representing a DNS message.
 */
typedef struct dns_message {
    dns_header_t *header;       /**< The DNS header. */
    forward_list_t questions;   /**< The list of questions. */
    forward_list_t answers;     /**< The list of answers. */
    forward_list_t authorities; /**< The list of authorities. */
    forward_list_t additionals; /**< The list of additionals. */
} dns_message_t;

/**
 * @brief Create a name field.
 *
 * @param name The name.
 * @param length The length of the name.
 * @return A pointer to the created name field.
 */
name_field_t *name_field_create(const char *const name, size_t length);

/**
 * @brief Get the name from a name field.
 *
 * @param name_field The name field.
 * @return The name.
 */
char *get_name_from_name_field(const name_field_t *const name_field);

/**
 * @brief Parse a DNS message from a string.
 *
 * @param base The string containing the DNS message.
 * @return A pointer to the parsed DNS message.
 */
dns_message_t *parse_dns_message(const char *const base);

/**
 * @brief Convert a DNS message to a byte stream.
 *
 * @param dns_message The DNS message.
 * @param buffer The buffer to store the byte stream.
 * @return A pointer to the byte stream.
 */
uint8_t *convert_dns_message_to_stream(const dns_message_t *const dns_message, uint8_t *const buffer);

/**
 * @brief Clone a DNS header.
 *
 * @param dns_header The DNS header to clone.
 * @return A pointer to the cloned DNS header.
 */
dns_header_t *clone_dns_header(const dns_header_t *const dns_header);

/**
 * @brief Clone a name field.
 *
 * @param name_field The name field to clone.
 * @return A pointer to the cloned name field.
 */
name_field_t *clone_name_field(const name_field_t *const name_field);

/**
 * @brief Clone a question.
 *
 * @param q The question to clone.
 * @return A pointer to the cloned question.
 */
void *clone_question(void *q);

/**
 * @brief Clone a resource record.
 *
 * @param q The resource record to clone.
 * @return A pointer to the cloned resource record.
 */
void *clone_resource_record(void *q);

/**
 * @brief Clone a DNS message.
 *
 * @param dns_message The DNS message to clone.
 * @return A pointer to the cloned DNS message.
 */
dns_message_t *clone_dns_message(const dns_message_t *const dns_message);

/**
 * @brief Destroy a name field.
 *
 * @param p The name field to destroy.
 */
void name_field_destroy(name_field_t *p);

/**
 * @brief Destroy a DNS header.
 *
 * @param q The DNS header to destroy.
 */
void dns_header_destroy(dns_header_t *q);

/**
 * @brief Destroy a question.
 *
 * @param q The question to destroy.
 */
void question_destroy(void *q);

/**
 * @brief Destroy a resource record.
 *
 * @param q The resource record to destroy.
 */
void resource_record_destroy(void *q);

/**
 * @brief Destroy a DNS message.
 *
 * @param p The DNS message to destroy.
 */
void dns_message_destroy(dns_message_t *p);

#endif
