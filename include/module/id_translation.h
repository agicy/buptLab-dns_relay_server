/**
 * @file id_translation.h
 * @brief This file provides functions for ID translation.
 */

#pragma once
#ifndef ID_TRANSLATION_H
#define ID_TRANSLATION_H

#include <netinet/in.h>
#include <stdint.h>

/**
 * @brief Create a new ID.
 *
 * This function creates a new unique ID for a client.
 *
 * @return The new ID.
 */
uint16_t nid_create(void);

/**
 * @brief Set the original ID for a given ID.
 *
 * This function sets the original ID for a given ID.
 *
 * @param nid The given ID.
 * @param original_id The original ID to be set.
 */
void set_original_id(uint16_t nid, uint16_t original_id);

/**
 * @brief Set the client address for a given ID.
 *
 * This function sets the client address for a given ID.
 *
 * @param nid The given ID.
 * @param address The client address to be set.
 */
void set_client_address(uint16_t nid, const struct sockaddr_in *const address);

/**
 * @brief Get the original ID for a given ID.
 *
 * This function gets the original ID for a given ID.
 *
 * @param nid The given ID.
 * @return The original ID.
 */
uint16_t get_original_id(uint16_t nid);

/**
 * @brief Get the client address for a given ID.
 *
 * This function gets the client address for a given ID.
 *
 * @param nid The given ID.
 * @return The client address.
 */
struct sockaddr_in *get_client_address(uint16_t nid);

/**
 * @brief Release a given ID.
 *
 * This function releases a given ID and its associated resources.
 *
 * @param nid The given ID.
 */
void nid_release(uint16_t nid);

#endif
