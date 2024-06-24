/**
 * @file rule_table.h
 * @brief This file provides functions for managing a rule table.
 */

#pragma once
#ifndef RULE_TABLE_H
#define RULE_TABLE_H

#include "data_structure/forward_list.h"

#include <stdbool.h>

/** @brief IP address that represents a banned IP. */
static const char *const banned_ip = "0.0.0.0";

/**
 * @brief Load a rule table from a file.
 *
 * @param filename The name of the file containing the rule table.
 */
void load_rule_table(const char *const filename);

/**
 * @brief Check if a name is banned.
 *
 * @param name The name to check.
 * @return true if the name is banned, false otherwise.
 */
bool is_banned(const char *const name);

/**
 * @brief Get the configuration for a given name.
 *
 * @param name The name to get the configuration for.
 * @return A list of configurations for the given name.
 */
forward_list_t get_configured(const char *const name);

#endif
