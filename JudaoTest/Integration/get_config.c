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

    printf("Document root: %s\n", config->documentRoot);
    printf("Log file is %s\n", config->logFile);
    printf("404 locations is %s\n", config->location404);
    printf("Max connections is %d\n", config->maxConnections);
    printf("The port number is %d\n", config->port);

    cleanUpStruct(config);
    free(config);
    exit(EXIT_SUCCESS);
}
