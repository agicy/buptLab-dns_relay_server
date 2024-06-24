/**
 * @file dns_cache.h
 * @brief Header file for DNS cache module.
 */

#pragma once
#ifndef DNS_CACHE_H
#define DNS_CACHE_H

#include "data_structure/forward_list.h"
#include "data_structure/trie.h"
#include "network/dns_utility.h"

/**
 * @brief Initializes the DNS cache.
 *
 * @param item_limit The maximum number of items that the cache can hold.
 */
void dns_cache_init(size_t item_limit);

/**
 * @brief Inserts a question and its corresponding resource record into the DNS cache.
 *
 * @param question Pointer to the question to be inserted.
 * @param resource_record Pointer to the resource record to be inserted.
 */
void dns_cache_insert(const question_t *const question, const resource_record_t *const resource_record);

/**
 * @brief Queries the DNS cache for a specific question.
 *
 * @param question Pointer to the question to be queried.
 * @return Pointer to the node in the forward list containing the query result.
 */
forward_list_node_t *dns_cache_query(const question_t *const question);

#endif
