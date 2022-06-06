/**
*
* Author: C. Fernandes
* clintonf@gmail.com
* Copyright (c) 2020. All rights reserved.
**/

#ifndef HTTP_SERVER_GET_CONFIGURATION_H
#define HTTP_SERVER_GET_CONFIGURATION_H
/**
 * This struct holds all of the configuration variables obtained
 * from reading in the config file, and/or command line options, and/or environment variables
 */
struct configuration {
    int port;
    int maxConnections;
    char* file404;
    char* serverRoot;
    char* logFile;
    char* serverName;
    char* defaultDocument;
    int serverMode;
};

/**
 * Frees struct memory.
 *
 * @param config struct configuration*
 */
void cleanUpStruct(struct configuration* config);

/**
 * Sets a value for the configuration object.
 *
 * @param configValue config object part to set
 * @param valueToAssign value to set
 */
void assignConfigStringValue(char** configValue, const char* valueToAssign);

/**
 * Sets a value for the configuration object.
 *
 * @param configValue config object part to set
 * @param valueToAssign value to set
 */
void assignConfigIntValue(int* configValue, const int valueToAssign);

/**
 * Sets a value for the configuration object from an environment variable.
 * Checks for existence of the variable before assignment. If no such environment variable exists
 * nothing happens.
 *
 * @param configValue config object part to set
 * @param environmentVariableName environment variable name
 */
void assignConfigStringFromEnvironment(char** configValue, const char* environmentVariableName);

/**
 * Sets a value for the configuration object from an environment variable.
 * Checks for existence of the variable before assignment. If no such environment variable exists
 * nothing happens.
 *
 * @param configValue config object part to set
 * @param environmentVariableName environment variable name
 */
void assignConfigIntFromEnvironment(int *configValue, const char *environmentVariableName);

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
 * Prints out usage information for the program.
 */
void usage();

/**
 * Reads the configuration file to set server variables.
 *
 * @return struct configuration*
 */
struct configuration *readConfiguration(const int argCount, const char **args);
#endif //HTTP_SERVER_GET_CONFIGURATION_H
