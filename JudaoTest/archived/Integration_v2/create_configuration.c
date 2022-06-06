/**
*
* Author: C. Fernandes
* clintonf@gmail.com
* Copyright (c) 2020. All rights reserved.
**/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libconfig.h"

#include "config_defaults.h"
#include "create_configuration.h"

int getFileDescriptor() {
    char* filename = DEFAULT_CONFIGURATION_FILE;

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            return ENOENT;
        }
    }

    return fd;
}

void createConfigWithDefaults() {
    char* configFile = DEFAULT_CONFIGURATION_FILE;
    config_t cfg;
    config_setting_t *root, *setting;

    printf("Creating configuration file using defaults\n");
    printf("Creating default config at %s\n", configFile);

    config_init(&cfg);
    root = config_root_setting(&cfg);

    //Defaults

    setting = config_setting_add(root, "document_root", CONFIG_TYPE_STRING);
    config_setting_set_string(setting, DEFAULT_DOCUMENT_ROOT);

    setting = config_setting_add(root, "error_404", CONFIG_TYPE_STRING);
    config_setting_set_string(setting, DEFAULT_404);

    setting = config_setting_add(root, "logfile_location", CONFIG_TYPE_STRING);
    config_setting_set_string(setting, DEFAULT_LOG_FILE);

    setting = config_setting_add(root, "port", CONFIG_TYPE_INT);
    config_setting_set_int(setting, DEFAULT_PORT);

    setting = config_setting_add(root, "max_connections", CONFIG_TYPE_INT);
    config_setting_set_int(setting, DEFAULT_MAX_CONNECTIONS);



    //WRITE OUT CONFIG FILE
    if(! config_write_file(&cfg, configFile)) {
        fprintf(stderr, "Failure writing file\n");
        config_destroy(&cfg);
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Config successfully written to %s\n", configFile);
    config_destroy(&cfg);
}

//TODO: Delete
void createMiniConfig() {
    const char* outputFile = "test_config.txt";
    config_t cfg;
    config_setting_t *root, *setting;

    config_init(&cfg);
    root = config_root_setting(&cfg);

    setting = config_setting_add(root, "bestBoy", CONFIG_TYPE_STRING);
    config_setting_set_string(setting, "HoboCat");

    setting = config_setting_add(root, "bestGirl", CONFIG_TYPE_STRING);
    config_setting_set_string(setting, "CallieCat");

    if(! config_write_file(&cfg, outputFile)) {
        fprintf(stderr, "Failure writing file\n");
        config_destroy(&cfg);
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Config successfully written to %s\n", outputFile);
    config_destroy(&cfg);
    exit(EXIT_SUCCESS);
}
