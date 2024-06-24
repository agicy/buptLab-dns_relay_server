#include "module/cmd_interpreter.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cmd_opt_t get_options(int argc, char *argv[]) {
    cmd_opt_t options = {
        .debug_level = 0,
        .cache_size = -1,
        .listen_port = 53,
        .hosts_file_name = "hosts.txt",
        .isp_dns_server_ip = "114.114.114.114",
        .log_file_name = "dns_relay.log",
        .stderr_enable = false};

    struct option long_options[] = {
        {"debug-level", required_argument, NULL, 'd'},
        {"cache-size", required_argument, NULL, 'c'},
        {"listen-port", required_argument, NULL, 'p'},
        {"file-hosts", required_argument, NULL, 'f'},
        {"dns-server", required_argument, NULL, 's'},
        {"log-file", required_argument, NULL, 'l'},
        {"stderr-enable", no_argument, NULL, 'e'},
        {NULL, 0, NULL, 0}};

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "d:c:p:f:s:l:e", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'd':
            if (optarg)
                options.debug_level = strtoul(optarg, NULL, 10);
            else
                options.debug_level = 0;
            break;
        case 'c':
            if (optarg)
                options.cache_size = strtoul(optarg, NULL, 10);
            else
                options.cache_size = 512;
            break;
        case 'p':
            if (optarg)
                options.listen_port = strtoul(optarg, NULL, 10);
            else
                options.listen_port = 512;
            break;
        case 'f':
            if (optarg)
                options.hosts_file_name = strdup(optarg);
            else {
                fprintf(stderr, "Missing argument for hosts file.\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 's':
            if (optarg)
                options.isp_dns_server_ip = strdup(optarg);
            else {
                fprintf(stderr, "Missing argument for DNS server IP.\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'l':
            if (optarg)
                options.log_file_name = strdup(optarg);
            else {
                fprintf(stderr, "Missing argument for log file.\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'e': {
            options.stderr_enable = true;
            break;
        }
        default:
            fprintf(stderr, "Usage: %s [-d debug-level] [-c cache-size] [-p listen-port] [-h hosts-file] [-s dns-server] [-l log-file] [-e stderr-enable]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    return options;
}
