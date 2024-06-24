/**
 * @file logger.h
 * @brief This file provides functions for logging.
 */

#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include "network/dns_utility.h"

#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Enumeration of log levels.
 */
typedef enum log_level {
    LOG_LEVEL_ERROR,   /**< Error level log. */
    LOG_LEVEL_WARNING, /**< Warning level log. */
    LOG_LEVEL_INFO,    /**< Info level log. */
    LOG_LEVEL_DEBUG,   /**< Debug level log. */
} log_level;

/**
 * @brief Initialize the logger.
 *
 * @param filename The name of the log file.
 * @param debug_level The debug level.
 * @param stderr_enable Whether to enable stderr.
 */
void logger_init(const char *const filename, const size_t debug_level, const bool stderr_enable);

/**
 * @brief Write a log.
 *
 * @param level The log level.
 * @param format The format string.
 * @return The number of characters written if successful, or a negative value if an error occurs.
 */
int logger_write(const log_level level, const char *const format, ...);

/**
 * @brief Log hex data.
 *
 * @param level The log level.
 * @param binary_string The binary string to log.
 * @param len The length of the binary string.
 */
void logger_hex(const log_level level, const uint8_t *binary_string, const size_t len);

/**
 * @brief Log a DNS message.
 *
 * @param level The log level.
 * @param dns_message The DNS message to log.
 */
void logger_dns_message(const log_level level, const dns_message_t *dns_message);

#endif
