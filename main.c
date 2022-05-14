#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "sub.h"

#define PORT 5678

int main() {

    int rfd; // Rendevouz-Descriptor

    // Socket erstellen
    rfd = socket(AF_INET, SOCK_STREAM, 0);
    if (rfd < 0 ){
        fprintf(stderr, "socket konnte nicht erstellt werden\n");
        exit(EXIT_FAILURE);
    }


    // Socket Optionen setzen für schnelles wiederholtes Binden der Adresse
    int option = 1;
    setsockopt(rfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));


    // Socket binden
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    int brt = bind(rfd, (struct sockaddr *) &server, sizeof(server));
    if (brt < 0 ){
        fprintf(stderr, "socket konnte nicht gebunden werden\n");
        exit(EXIT_FAILURE);
    }


    // Socket lauschen lassen
    int lrt = listen(rfd, 5);
    if (lrt < 0 ){
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(EXIT_FAILURE);
    }

    int storageID = shmget(IPC_PRIVATE, sizeof(Storage), IPC_CREAT | 600);
    if(storageID < 0) {
        perror("Can't create shared storage");
        exit(EXIT_FAILURE);
    }
    void* shmMem = shmat(storageID, NULL, 0);
    if (shmctl(storageID, IPC_RMID, NULL) < 0) {
        perror("Can't mark shared storage for auto remove");
        exit(EXIT_FAILURE);
    }
    if (shmctl(storageID, SHM_UNLOCK, NULL) < 0) {
        perror("Can't mark shared storage for auto swap out by process ending");
        exit(EXIT_FAILURE);
    }
    if (shmMem == (void *) 1) {
        perror("Can't attache shared storage");
        exit(EXIT_FAILURE);
    }
    Storage* storage = (Storage *) shmMem;
    storage_init(storage);

    int storageSemID = semget(IPC_PRIVATE, 1, IPC_CREAT | 600);
    if (storageSemID < 0) {
        perror("Can't create semaphore for shared storage");
        exit(EXIT_FAILURE);
    }


    run(rfd, storageID);

    // Rendevouz Descriptor schließen
    shutdown(rfd, SHUT_RDWR);
    close(rfd);

}