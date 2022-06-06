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

void usage(const char *program) {
    char help[400];

    sprintf(help, "Usage: %s [options]\n"
                 "-r --server-root <root directory for server>\n"
                 "-s --server-name <server name>\n"
                 "-l --log-file <log file name>\n"
                 "-n --location-404 <404 file location>\n"
                 "-d --default-document <default file response>\n"
                 "-c --max-connections <max # of server connections>\n"
                 "-p --port <port # to listen on>\n"
                 "-m --server-mode \n"
                 "-h --help\n\n", program);

    char *moreHelp = "All arguments are optional and may be specified in several methods (in order of precedence):\n"
                     "command line --> configuration file (config.conf) --> environment variables --> defaults\n\n";

    char* serverMode = "Server mode is an integer value, 1 = threads, 2 = processes\n\n";

    char* environmentVariables = "Environment variable list:\n"
                                 "server root: DC_SERVER_ROOT\n"
                                 "log filename: DC_LOG_FILE\n"
                                 "404 filename: DC_404_FILE\n"
                                 "server name: DC_SERVER_NAME\n"
                                 "default document: DC_DEFAULT_DOCUMENT\n"
                                 "maximum connections: DC_MAX_CONNECTIONS\n"
                                 "port number: DC_PORT\n"
                                 "server mode: DC_SERVER_MODE\n\n";

    printf("%s", help);
    printf("%s", moreHelp);
    printf("%s", environmentVariables);
    printf("%s", serverMode);
    exit(EXIT_SUCCESS);
}

void cleanUpStruct(struct configuration* config) {
    if (config->serverRoot) {
        free(config->serverRoot);
    }

    if (config->logFile) {
        free(config->logFile);
    }

    if (config->file404) {
        free(config->file404);
    }

    if (config->serverName) {
        free(config->serverName);
    }

    if (config->defaultDocument) {
        free(config->defaultDocument);
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

void assignConfigIntFromEnvironment(int *configValue, const char *environmentVariableName) {
    if (getenv(environmentVariableName)) {
        char* str;
        int value;

        value = (int) strtol( getenv(environmentVariableName), &str, 10);

        assignConfigIntValue(configValue, value);
    }
}

void assignConfigIntValue(int* configValue, const int valueToAssign) {
    if (! *configValue) {
        *configValue = valueToAssign;
    }
}

void readConfigurationFromCommandLine(struct configuration *configSoFar, const int argCount, const char **args) {
    int opt;
    char error[80];
    sprintf(error, "Invalid option. Try %s --help\n", args[0]);
    char* str = NULL;

    while (1) {
        int optionIndex = 0;

        static struct option longOptions[] = {
                {"server-root",          required_argument, 0, 'r'},
                {"log-file",             required_argument, 0, 'l'},
                {"location-404",         required_argument, 0, 'n'},
                {"server-name",          required_argument, 0, 's'},
                {"default-document",     required_argument, 0, 'd'},
                {"max-connections",      required_argument, 0, 'c'},
                {"port",                 required_argument, 0, 'p'},
                {"server-mode",          required_argument, 0, 'm'},
                {"help",                 no_argument,       0, 'h'},
                {0,              0,                 0,  0 }
        };

        opt = getopt_long(argCount, (char *const *) args, "r:l:n:c:p:s:m:h", longOptions, &optionIndex);

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
                assignConfigStringValue(&configSoFar->file404, optarg);
                break;
            case 's':
                assignConfigStringValue(&configSoFar->serverName, optarg);
                break;
            case 'd':
                assignConfigStringValue(&configSoFar->defaultDocument, optarg);
                break;
            case 'c':
                configSoFar->maxConnections = (int) strtol( optarg, &str, 10);
                break;
            case 'p':
                configSoFar->port = (int) strtol( optarg, &str, 10);
                break;
            case 'm':
                configSoFar->serverMode = (int) strtol( optarg, &str, 10);
                break;
            case 'h':
                usage(args[0]);
               break;
            default:
                fprintf(stderr, error, args[0]);
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

    if (! configSoFar->file404) {
        const char* locationOf404;
        configLookupResult = config_lookup_string(&cfg, "error_404", &locationOf404);

        if (configLookupResult == CONFIG_TRUE) {
            configSoFar->file404 = strdup(locationOf404);
        } else {
            perror("Failed to read 404 file configuration");
        }
    }

    if (! configSoFar->defaultDocument) {
        const char* s;
        configLookupResult = config_lookup_string(&cfg, "default_document", &s);

        if (configLookupResult == CONFIG_TRUE) {
            configSoFar->defaultDocument = strdup(s);
        } else {
            perror("Failed to read default document configuration");
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

    if (! configSoFar->serverMode) {
        int mode;
        configLookupResult = config_lookup_int(&cfg, "server_mode", &mode);

        if (configLookupResult == CONFIG_TRUE) {
            configSoFar->serverMode = mode;
        } else {
            perror("Failed to read server mode");
        }
    }

    config_destroy(&cfg);
}

void readConfigurationFromEnvironment(struct configuration* configSoFar) {
    assignConfigStringFromEnvironment(&configSoFar->serverRoot, "DC_SERVER_ROOT");
    assignConfigStringFromEnvironment(&configSoFar->logFile, "DC_LOG_FILE");
    assignConfigStringFromEnvironment(&configSoFar->file404, "DC_404_FILE");
    assignConfigStringFromEnvironment(&configSoFar->serverName, "DC_SERVER_NAME");
    assignConfigStringFromEnvironment(&configSoFar->defaultDocument, "DC_DEFAULT_DOCUMENT");

    assignConfigIntFromEnvironment(&configSoFar->maxConnections, "DC_MAX_CONNECTIONS");

    assignConfigIntFromEnvironment(&configSoFar->port, "DC_SERVER_PORT");

    assignConfigIntFromEnvironment(&configSoFar->serverMode, "DC_SERVER_MODE");
}

void readConfigurationFromDefaults(struct configuration *configSoFar) {
    assignConfigStringValue(&configSoFar->serverRoot, DEFAULT_SERVER_ROOT);
    assignConfigStringValue(&configSoFar->logFile, DEFAULT_LOG_FILE);
    assignConfigStringValue(&configSoFar->file404, DEFAULT_404);
    assignConfigStringValue(&configSoFar->serverName, DEFAULT_SERVER_NAME);
    assignConfigStringValue(&configSoFar->defaultDocument, DEFAULT_DOCUMENT);

    assignConfigIntValue(&configSoFar->maxConnections, DEFAULT_MAX_CONNECTIONS);

    assignConfigIntValue(&configSoFar->port, DEFAULT_PORT);

    if (! configSoFar->port) {
        configSoFar->port = DEFAULT_PORT;
    }

    assignConfigIntValue(&configSoFar->serverMode, DEFAULT_SERVER_MODE);
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
