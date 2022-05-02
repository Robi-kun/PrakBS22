//
// Created by AI Robin on 22.04.2022.
//

#ifndef PRAKBS22_SUB_H
#define PRAKBS22_SUB_H

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "main.h"

int sonderzeichen(char *string);

/*
 * Handels connection with client
 *
 * RETURN:
 * -1: Can't receive massage from client
 * 1: Client ended session
 */
int connect_handle(int cfd);

#endif //PRAKBS22_SUB_H
