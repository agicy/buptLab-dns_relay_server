/**
 * @file trie.h
 * @brief Header file for a trie data structure.
 */

#pragma once
#ifndef TRIE_H
#define TRIE_H

#include <stddef.h>
#include <stdint.h>

/**
 * @def TRIE_RADIX
 * @brief The radix (or base) of the trie.
 *
 * This macro defines the radix (or base) of the trie. In this case, the radix is 256.
 */
#define TRIE_RADIX 256

/**
 * @typedef trie_radix_t
 * @brief Type for trie radix.
 *
 * This defines a type for the trie radix.
 */
typedef uint8_t trie_radix_t;

/**
 * @struct trie_node
 * @brief A node in the trie.
 *
 * This structure represents a single node in the trie. Each node contains a count, a pointer to the parent node, an array of child nodes, and a value.
 */
typedef struct trie_node {
    size_t count;                     /**< The count of the node. */
    struct trie_node *fa;             /**< Pointer to the parent node. */
    struct trie_node *ch[TRIE_RADIX]; /**< Array of child nodes. */
    void *value;                      /**< The value stored in the node. */
} trie_node_t;

/**
 * @typedef trie_t
 * @brief Type for trie.
 *
 * This defines a type for the trie.
 */
typedef trie_node_t *trie_t;

/**
 * @brief Creates a new trie.
 *
 * @return Pointer to the newly created trie.
 */
trie_t trie_create(void);

/**
 * @brief Destroys a trie.
 *
 * @param p Pointer to the trie to be destroyed.
 * @param value_destroy Function pointer to the function used to destroy the value in the nodes.
 */
void trie_destroy(trie_t p, void (*value_destroy)(void *));

/**
 * @brief Finds a node in the trie.
 *
 * @param rt Pointer to the root of the trie.
 * @param binary_string Pointer to the binary string to be found.
 * @param len Length of the binary string.
 * @return Pointer to the found node.
 */
trie_node_t *trie_find(trie_t rt, const trie_radix_t *binary_string, size_t len);

/**
 * @brief Inserts a binary string into the trie.
 *
 * @param rt Pointer to the root of the trie.
 * @param binary_string Pointer to the binary string to be inserted.
 * @param len Length of the binary string.
 * @return Pointer to the inserted node.
 */
trie_node_t *trie_insert(trie_t rt, const trie_radix_t *binary_string, size_t len);

#endif
