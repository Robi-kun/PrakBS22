//
// Created by AI Robin on 22.04.2022.
//

#ifndef PRAKBS22_SUB_H
#define PRAKBS22_SUB_H


#include "keyValStore.h"


#define BUFFSIZE 1024 // Größe des Buffers

/*
 * Handels application logic
 * RETURN:
 * 0: No Errors
 */
void run(int serverFd, int storageID);

#endif //PRAKBS22_SUB_H
