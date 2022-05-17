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

    int storageID = shmget(IPC_PRIVATE, sizeof(Storage), IPC_CREAT | 0600);
    if(storageID < 0) {
        perror("Can't create shared storage");
        exit(EXIT_FAILURE);
    }
    printf("Shared storage ID: %i\n", storageID);
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

    Sem_Config storageSem;
    storageSem.ID = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if (storageSem.ID < 0) {
        perror("Can't create semaphore for shared storage");
        exit(EXIT_FAILURE);
    }
    unsigned short storageSemValues[1];
    storageSemValues[0] = 1;
    if(semctl(storageSem.ID, 1, SETALL, storageSemValues) < 0) {
        perror("Can't initialize semaphore");
        semctl(storageSem.ID, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    printf("Storage semaphore ID: %i\n", storageSem.ID);
    storageSem.up.sem_num = storageSem.down.sem_num = 0;
    storageSem.up.sem_flg = storageSem.down.sem_flg = SEM_UNDO;
    storageSem.up.sem_op = 1;
    storageSem.down.sem_op = -1;

    storage_init(storage);

    Config config;
    config.serverFd = rfd;
    config.storageId = storageID;
    config.storageSem = storageSem;

    run(&config);

    shmdt(storage);
    if (semctl(storageSem.ID, 1, IPC_RMID) < 0) {
        perror("Can't remove the storage semaphore");
        exit(EXIT_FAILURE);
    }
    printf("Removed storage semaphore group with ID: %i\n", storageSem.ID);

    // Rendevouz Descriptor schließen
    shutdown(rfd, SHUT_RDWR);
    close(rfd);

}