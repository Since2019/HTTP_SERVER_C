/**
*
* Author: C. Fernandes
* clintonf@gmail.com
* Copyright (c) 2020. All rights reserved.
**/

#include <stdbool.h>

#ifndef HTTP_SERVER_CREATE_CONFIGURATION_H
#define HTTP_SERVER_CREATE_CONFIGURATION_H

/**
 * Checks if a file exists.
 *
 * @return bool
 */
bool doesFileExist();

/**
 * Gets the config file file descriptor.
 *
 *
 *  @return int ENOENT is returned if the file cannot be found (i.e. it needs to be created)
 */
int getFileDescriptor();

/**
 * Creates a configuration file in the default location, with default settings.
 *
 * @return int
 */
void createConfigWithDefaults();

void createMiniConfig(); //TODO: delete

#endif //HTTP_SERVER_CREATE_CONFIGURATION_H
