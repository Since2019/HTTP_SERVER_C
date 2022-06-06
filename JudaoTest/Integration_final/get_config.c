/**
*
* Author: C. Fernandes
* clintonf@gmail.com
* Copyright (c) 2020. All rights reserved.
**/

#include <stdlib.h>
#include <stdio.h>

#include "get_configuration.h"

int main(int argc, char** argv) {
    struct configuration* config = readConfiguration(argc, (const char **) argv);

    printf("Server root: %s\n", config->serverRoot);
    printf("Log file is %s\n", config->logFile);
    printf("404 locations is %s\n", config->file404);
    printf("Max connections is %d\n", config->maxConnections);
    printf("The port number is %d\n", config->port);
    printf("The server name is %s\n", config->serverName);
    printf("The default document is %s\n", config->defaultDocument);
    printf("The server mode is %d\n", config->serverMode);

    cleanUpStruct(config);
    free(config);
    exit(EXIT_SUCCESS);
}
