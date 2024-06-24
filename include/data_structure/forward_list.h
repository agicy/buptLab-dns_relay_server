/**
 * @file forward_list.h
 * @brief Header file for a forward list data structure.
 */

#pragma once
#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H

/**
 * @struct forward_list_node
 * @brief A node in the forward list.
 *
 * This structure represents a single node in the forward list. Each node contains a value and a pointer to the next node.
 */
typedef struct forward_list_node {
    void *value;                    /**< The value stored in the node. */
    struct forward_list_node *next; /**< Pointer to the next node in the list. */
} forward_list_node_t;

/**
 * @typedef forward_list_t
 * @brief Type for forward list.
 *
 * This defines a type for the forward list.
 */
typedef forward_list_node_t *forward_list_t;

/**
 * @brief Creates a new node for the forward list.
 *
 * @param next Pointer to the next node.
 * @return Pointer to the newly created node.
 */
forward_list_node_t *forward_list_node_create(forward_list_node_t *next);

/**
 * @brief Destroys a node in the forward list.
 *
 * @param p Pointer to the node to be destroyed.
 * @param value_destroy Function pointer to the function used to destroy the value in the node.
 */
void forward_list_node_destroy(forward_list_node_t *p, void (*value_destroy)(void *));

/**
 * @brief Creates a new forward list.
 *
 * @return Pointer to the newly created forward list.
 */
forward_list_t forward_list_create();

/**
 * @brief Destroys a forward list.
 *
 * @param p Pointer to the forward list to be destroyed.
 * @param value_destroy Function pointer to the function used to destroy the value in the nodes.
 */
void forward_list_destroy(forward_list_t p, void (*value_destroy)(void *));

/**
 * @brief Deletes a node from a forward list.
 *
 * @param p Pointer to the forward list.
 * @param q Pointer to the node to be deleted.
 * @param value_destroy Function pointer to the function used to destroy the value in the node.
 * @return Pointer to the forward list after deletion.
 */
forward_list_t forward_list_delete(forward_list_t p, forward_list_node_t *q, void (*value_destroy)(void *));

/**
 * @brief Clones a forward list.
 *
 * @param p Pointer to the forward list to be cloned.
 * @param value_clone Function pointer to the function used to clone the value in the nodes.
 * @return Pointer to the cloned forward list.
 */
forward_list_t forward_list_clone(forward_list_t p, void *(*value_clone)(void *));

#endif
