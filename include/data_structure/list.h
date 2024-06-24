/**
 * @file list.h
 * @brief Header file for a doubly linked list data structure.
 */

#pragma once
#ifndef LIST_H
#define LIST_H

/**
 * @struct list_node
 * @brief A node in the doubly linked list.
 *
 * This structure represents a single node in the doubly linked list. Each node contains a value and pointers to the previous and next nodes.
 */
typedef struct list_node {
    void *value;            /**< The value stored in the node. */
    struct list_node *prev; /**< Pointer to the previous node in the list. */
    struct list_node *next; /**< Pointer to the next node in the list. */
} list_node_t;

#endif
