//
// Created by AI Robin on 22.04.2022.
//

#ifndef PRAKBS22_SUB_H
#define PRAKBS22_SUB_H

#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "keyValStore.h"


#define BUFFSIZE 1024 // Größe des Buffers

typedef struct Sem_Config {
    int ID;
    struct sembuf up;
    struct sembuf down;
} Sem_Config;

typedef struct Config {
    int serverFd;
    int storageId;
    Sem_Config storageSem;
} Config;


/*
 * Handels application logic
 * RETURN:
 * 0: No Errors
 */
void run(Config* config);

#endif //PRAKBS22_SUB_H
