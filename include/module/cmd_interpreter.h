/**
 * @file cmd_interpreter.h
 * @brief Header file for command line options interpreter.
 */

#pragma once
#ifndef CMD_INTERPRETER_H
#define CMD_INTERPRETER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @struct cmd_opt
 * @brief A structure to hold command line options.
 *
 * This structure represents the command line options. Each option is represented by a member in the structure.
 */
typedef struct cmd_opt {
    size_t debug_level;            /**< The debug level. */
    size_t cache_size;             /**< The cache size. */
    uint16_t listen_port;          /**< The listening port number. */
    const char *hosts_file_name;   /**< The name of the hosts file. */
    const char *isp_dns_server_ip; /**< The IP address of the ISP DNS server. */
    const char *log_file_name;     /**< The name of the log file. */
    bool stderr_enable;            /**< Flag to enable standard error output. */
} cmd_opt_t;

/**
 * @brief Function to get command line options.
 *
 * @param argc The number of command line arguments.
 * @param argv The array of command line arguments.
 * @return The command line options.
 */
cmd_opt_t get_options(int argc, char *argv[]);

#endif
