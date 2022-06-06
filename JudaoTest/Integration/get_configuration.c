/**
*
* Author: C. Fernandes
* clintonf@gmail.com
* Copyright (c) 2020. All rights reserved.
**/

#include <getopt.h>
#include <libconfig.h>
#include <stdlib.h>
#include <string.h>

// #include "config_defaults.h"
#include "get_configuration.h"

void cleanUpStruct(struct configuration* config) {
    if (config->documentRoot) {
        free(config->documentRoot);
    }

    if (config->logFile) {
        free(config->logFile);
    }

    if (config->location404) {
        free(config->location404);
    }
}

void readConfigurationFromCommandLine(struct configuration *configSoFar, const int argCount, const char **args) {
    int opt;

    while ((opt = getopt(argCount, (char *const *) args, "r:l:n:c:p:") ) != -1 ) {
        switch (opt) {
            case 'r':
                configSoFar->documentRoot = malloc(sizeof(char) * strlen(optarg) + 1);
                memset(configSoFar->documentRoot, 0, sizeof(char) * strlen(optarg) + 1);
                strcpy(configSoFar->documentRoot, optarg);
                break;
            case 'l':
                configSoFar->logFile = malloc(sizeof(char) * strlen(optarg) + 1);
                memset(configSoFar->logFile, 0, sizeof(char) * strlen(optarg) + 1);
                strcpy(configSoFar->logFile, optarg);
                break;
            case 'n':
                configSoFar->location404 = malloc(sizeof(char) * strlen(optarg) + 1);
                memset(configSoFar->location404, 0, sizeof(char) * strlen(optarg) + 1);
                strcpy(configSoFar->location404, optarg);
                break;
            case 'c':
                configSoFar->maxConnections = atoi(optarg);
                break;
            case 'p':
                configSoFar->port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -r <doc root> -l <log file path> -n <404 file path> -c <max number of connections> -p <port number>\n", args[0]);
                cleanUpStruct(configSoFar);
                free(configSoFar);
                exit(EXIT_FAILURE);
        }
    }
}

void readConfigurationFromFile(struct configuration* configSoFar) {
    char* configFile = DEFAULT_CONFIGURATION_FILE;
    config_t cfg;
    int configLookupResult = 0;

    config_init(&cfg);

    if(! config_read_file(&cfg, configFile)) {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        exit(EXIT_FAILURE);
    }

    if (! configSoFar->documentRoot) {
        const char* root;
        configLookupResult = config_lookup_string(&cfg, "document_root", &root);

        if (configLookupResult == CONFIG_TRUE) {
            configSoFar->documentRoot = strdup(root);
        } else {
            perror("Failed to read document root configuration");
        }
    }

    if (! configSoFar->logFile) {
        const char* log;
        configLookupResult = config_lookup_string(&cfg, "logfile_location", &log);

        if (configLookupResult == CONFIG_TRUE) {
            configSoFar->logFile = strdup(log);
        } else {
            perror("Failed to read log file configuration");
        }
    }

    if (! configSoFar->location404) {
        const char* locationOf404;
        configLookupResult = config_lookup_string(&cfg, "error_404", &locationOf404);

        if (configLookupResult == CONFIG_TRUE) {
            configSoFar->location404 = strdup(locationOf404);
        } else {
            perror("Failed to read 404 file configuration");
        }
    }

    if (! configSoFar->maxConnections) {
        int maxConn;

        configLookupResult = config_lookup_int(&cfg, "max_connections", &maxConn);

        if (configLookupResult == CONFIG_TRUE) {
            configSoFar->maxConnections = maxConn;
        } else {
            perror("Failed to read max connections configuration");
        }
    }

    if (! configSoFar->port) {
        int portNum;
        configLookupResult = config_lookup_int(&cfg, "port", &portNum);

        if (configLookupResult == CONFIG_TRUE) {
            configSoFar->port = portNum;
        } else {
            perror("Failed to read port configuration");
        }
    }

    config_destroy(&cfg);
}

void readConfigurationFromEnvironment(struct configuration* configSoFar) {;}

void readConfigurationFromDefaults(struct configuration *configSoFar) {
    if (! configSoFar->documentRoot) {
        configSoFar->documentRoot = malloc(sizeof(char) * strlen(DEFAULT_DOCUMENT_ROOT) + 1);
        memset(configSoFar->documentRoot, 0, sizeof(char) * strlen(DEFAULT_DOCUMENT_ROOT) + 1);
        strcpy(configSoFar->documentRoot, DEFAULT_DOCUMENT_ROOT);
    }

    if (! configSoFar->logFile) {
        configSoFar->logFile = malloc(sizeof(char) * strlen(DEFAULT_LOG_FILE) + 1);
        memset(configSoFar->logFile, 0, sizeof(char) * strlen(DEFAULT_LOG_FILE) + 1);
        strcpy(configSoFar->logFile, DEFAULT_LOG_FILE);
    }

    if (! configSoFar->location404) {
        configSoFar->location404 = malloc(sizeof(char) * strlen(DEFAULT_404) + 1);
        memset(configSoFar->location404, 0, sizeof(char) * strlen(DEFAULT_404) + 1);
        strcpy(configSoFar->location404, DEFAULT_404);
    }

    if (! configSoFar->maxConnections) {
        configSoFar->maxConnections = DEFAULT_MAX_CONNECTIONS;
    }

    if (! configSoFar->port) {
        configSoFar->port = DEFAULT_PORT;
    }
}

struct configuration *readConfiguration(const int argCount, const char **args) {
    struct configuration* config;
    config = malloc(sizeof(struct configuration));
    memset(config, 0, sizeof(struct configuration));

    readConfigurationFromCommandLine(config, argCount, args);
    readConfigurationFromFile(config);
    readConfigurationFromEnvironment(config);
    readConfigurationFromDefaults(config);

    return config;
}

