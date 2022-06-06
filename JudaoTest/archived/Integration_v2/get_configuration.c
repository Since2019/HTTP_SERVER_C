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
#include <unistd.h>

#include "config_defaults.h"
#include "get_configuration.h"

void cleanUpStruct(struct configuration* config) {
    if (config->serverRoot) {
        free(config->serverRoot);
    }

    if (config->logFile) {
        free(config->logFile);
    }

    if (config->location404) {
        free(config->location404);
    }

    if (config->serverName) {
        free(config->serverName);
    }
}

void assignConfigStringValue(char** configValue, const char* valueToAssign) {
    if (! *configValue) {
        *configValue = malloc(sizeof(char) * strlen(valueToAssign) + 1);
        memset(*configValue, 0, sizeof(char) * strlen(valueToAssign) + 1);
        strcpy(*configValue, valueToAssign);
    }
}

void assignConfigStringFromEnvironment(char **configValue, const char *environmentVariableName) {
    if (getenv(environmentVariableName)) {
        assignConfigStringValue(configValue, getenv(environmentVariableName));
    }
}

void readConfigurationFromCommandLine(struct configuration *configSoFar, const int argCount, const char **args) {
    int opt;
    char* str = NULL;

    while (1) {
        int optionIndex = 0;

        static struct option longOptions[] = {
                {"server-root",     required_argument, 0, 'r'},
                {"log-file",        required_argument, 0, 'l'},
                {"location-404",    required_argument, 0, 'n'},
                {"server-name",     required_argument, 0, 's'},
                {"max-connections", required_argument, 0, 'c'},
                {"port",            required_argument, 0, 'p'},
                {0,         0,                 0,  0 }
        };

        opt = getopt_long(argCount, (char *const *) args, "r:l:n:c:p:s:", longOptions, &optionIndex);

        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'r':
                assignConfigStringValue(&configSoFar->serverRoot, optarg);
                break;
            case 'l':
                assignConfigStringValue(&configSoFar->logFile, optarg);
                break;
            case 'n':
                assignConfigStringValue(&configSoFar->location404, optarg);
                break;
            case 's':
                assignConfigStringValue(&configSoFar->serverName, optarg);
                break;
            case 'c':
                configSoFar->maxConnections = (int) strtol( optarg, &str, 10);
                break;
            case 'p':
                configSoFar->port = (int) strtol( optarg, &str, 10);
                break;
            default:
                fprintf(stderr, "Usage: %s -r <server root> -s <server name> -l <log file path> -n <404 file path> -c <max number of connections> -p <port number>\n", args[0]);
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

    if (! configSoFar->serverRoot) {
        const char* root;
        configLookupResult = config_lookup_string(&cfg, "server_root", &root);

        if (configLookupResult == CONFIG_TRUE) {
            configSoFar->serverRoot = strdup(root);
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

    if (! configSoFar->serverName) {
        const char* serverName;
        configLookupResult = config_lookup_string(&cfg, "server_name", &serverName);

        if (configLookupResult == CONFIG_TRUE) {
            configSoFar->serverName = strdup(serverName);
        } else {
            perror("Failed to read server name configuration");
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

void readConfigurationFromEnvironment(struct configuration* configSoFar) {
    assignConfigStringFromEnvironment(&configSoFar->serverRoot, "DC_SERVER_ROOT");
    assignConfigStringFromEnvironment(&configSoFar->logFile, "DC_LOG_FILE");
    assignConfigStringFromEnvironment(&configSoFar->location404, "DC_404_LOCATION");
    assignConfigStringFromEnvironment(&configSoFar->serverName, "DC_SERVER_NAME");

    if (! configSoFar->maxConnections) {
        if (getenv("DC_MAX_CONNECTIONS")) {
            char* str;
            configSoFar->maxConnections = (int) strtol( getenv("DC_MAX_CONNECTIONS"), &str, 10);
        }
    }

    if (! configSoFar->port) {
        if (getenv("DC_SERVER_PORT")) {
            char* str;
            configSoFar->port = (int) strtol( getenv("DC_SERVER_PORT"), &str, 10);
        }
    }
}

void readConfigurationFromDefaults(struct configuration *configSoFar) {
    assignConfigStringValue(&configSoFar->serverRoot, DEFAULT_SERVER_ROOT);
    assignConfigStringValue(&configSoFar->logFile, DEFAULT_LOG_FILE);
    assignConfigStringValue(&configSoFar->location404, DEFAULT_404);
    assignConfigStringValue(&configSoFar->serverName, DEFAULT_SERVER_NAME);

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

    if( access(DEFAULT_CONFIGURATION_FILE, F_OK) != -1 ) {
        readConfigurationFromFile(config);
    }

    readConfigurationFromEnvironment(config);
    readConfigurationFromDefaults(config);

    return config;
}
