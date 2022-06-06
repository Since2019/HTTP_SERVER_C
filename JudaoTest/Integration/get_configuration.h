/**
*
* Author: C. Fernandes
* clintonf@gmail.com
* Copyright (c) 2020. All rights reserved.
**/

#ifndef HTTP_SERVER_GET_CONFIGURATION_H
#define HTTP_SERVER_GET_CONFIGURATION_H


/**
 * Default Configs:
 * If the config file does not exist,
 * the following configuration will be applied
*/
#define DEFAULT_DOCUMENT_ROOT "./"
#define DEFAULT_CONFIGURATION_FILE "./config.conf"
#define DEFAULT_LOG_FILE "./server.log"
#define DEFAULT_404 "./error_404.html"

#define DEFAULT_MAX_CONNECTIONS 10
#define DEFAULT_PORT 49157

/**
 * This struct holds all of the configuration variables obtained
 * from reading in the config file, and/or command line options, and/or environment variables
 */
struct configuration {
    int port;
    int maxConnections;
    char* location404;
    char* documentRoot;
    char* logFile;
};

/**
 * Frees struct memory.
 *
 * @param config struct configuration*
 */
void cleanUpStruct(struct configuration* config);

/**
 * Sets server environment from configuration file.
 *
 * @param configSoFar location for configuration settings
 */
void readConfigurationFromFile(struct configuration* configSoFar);

/**
 * Sets server environment from environment variables.
 *
 * @param configSoFar location for configuration settings
 */
void readConfigurationFromEnvironment(struct configuration* configSoFar);

/**
 * Sets server environment from command line arguments.
 *
 * @param configSoFar location for configuration settings
 * @param argCount number of arguments passed (argc)
 * @param args command line arguments (argv)
 */
void readConfigurationFromCommandLine(struct configuration *configSoFar, int argCount, const char **args);

/**
 * Sets configuration from hard-coded defaults;.
 *
 * @param configSoFar struct configuration*
 */
void readConfigurationFromDefaults(struct configuration *configSoFar);

/**
 * Reads the configuration file to set server variables.
 *
 * @return struct configuration*
 */
struct configuration *readConfiguration(const int argCount, const char **args);
#endif //HTTP_SERVER_GET_CONFIGURATION_H
