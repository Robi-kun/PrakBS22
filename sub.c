#include <signal.h>
#include "sub.h"
#include "connections.h"

typedef enum Command {
    PUT,
    GET,
    DEL,
    QUIT,
    FALSE
} Command;

Command interpret(char* command) {

    if(strncmp(command, "PUT", 3) == 0)
        return PUT;
    if(strncmp(command, "GET", 3) == 0)
        return GET;
    if(strncmp(command, "DEL", 3) == 0)
        return DEL;
    if(strncmp(command, "QUIT", 4) == 0)
        return QUIT;

    return FALSE;
}

int sonderzeichen(char *string){
    for(int i = 0; i < strlen(string); i++) {
        if (!((int) string[i] >= 48 && (int) string[i] <= 57
        || (int) string[i] >= 65 && (int) string[i] <= 90
        || (int) string[i] >= 97 && (int) string[i] <= 122
        || (int) string[i] == 32)) //ASCII: 48-57, 65-90, 97-122, 32
            return 0;
    }
    return 1;
}

/*
 * Handels connection with client
 *
 * RETURN:
 * -1: Can't receive massage from client
 * 1: Client ended session
 */
int connect_handle(int connectionFd, Sem_Config storageSem) {
    char in[BUFFSIZE], out[BUFFSIZE];

    while(1) {
        bzero(in, BUFFSIZE);
        bzero(out, BUFFSIZE);

        // Receive Command from client
        if(recv(connectionFd, in , BUFFSIZE, 0) == -1) {
            perror("ERROR: Can't receive massage from client");
            return -1;
        }

        // Interpret massage from client
        Command command = interpret(strtok(in, " "));

        switch (command) {
            case PUT: {
                char* key = strtok(NULL, " ");
                char* value = strtok(NULL, "\r");
                if(key == NULL || value == NULL) {
                    strcpy(out, "command_nonexistent");
                }
                else if(sonderzeichen(key) == 0 || sonderzeichen(value) == 0){
                    strcpy(out,"special_characters_not_allowed");
                }
                else {
                    printf("Key: %s\n", key);
                    printf("Value: %s\n", value);

                    strcpy(out, "PUT :");
                    strcat(out, key);
                    semop(storageSem.ID, &storageSem.down, 1);
                    if (put(key, value) > -1) {
                        semop(storageSem.ID, &storageSem.up, 1);
                        strcat(out, ":");
                        strcat(out, value);
                    } else {
                        semop(storageSem.ID, &storageSem.up, 1);
                        strcat(out, ":no_space_for_new_key");
                    }
                }
                break;
            };
            case GET: {
                char* key= strtok(NULL, "\r");

                if(key == NULL) {
                    strcpy(out, "command_nonexistent");
                }
                else {
                    char value[BUFFSIZE];

                    strcpy(out, "GET :");
                    strcat(out, key);
                    semop(storageSem.ID, &storageSem.down, 1);
                    if (get(key, value) == 1) {
                        semop(storageSem.ID, &storageSem.up, 1);
                        strcat(out, ":");
                        strcat(out, value);
                        puts(value);
                    } else {
                        semop(storageSem.ID, &storageSem.up, 1);
                        strcat(out, ":key_nonexistent");
                    }
                }
                break;
            };
            case DEL: {
                char* key= strtok(NULL, "\r");
                if(key== NULL) {
                    strcpy(out, "command_nonexistent");
                }
                else{
                    strcpy(out, "DEL :");
                    strcat(out, key);
                    semop(storageSem.ID, &storageSem.down, 1);
                    if (del(key) == 1) {
                        semop(storageSem.ID, &storageSem.up, 1);
                        strcat(out, ":key_deleted");
                    } else {
                        semop(storageSem.ID, &storageSem.up, 1);
                        strcat(out, ":key_nonexistent");
                    }
                }
                break;
            };
            case QUIT: {
                shutdown(connectionFd, SHUT_RDWR);
                close(connectionFd);
                return 1;
            };
            case FALSE: {
                strcpy(out,"command_nonexistent");
                break;
            };
            default:
                break;
        }
        // New line after output
        strcat(out, "\n");

        send(connectionFd, out, BUFFSIZE, 0);
    }
}

void new_process(int cfd, int storageId, Sem_Config storageSem) {
    void* shmMem = shmat(storageId, NULL, 0);
    if(shmMem == (void *) 1) {
        perror("Can't attach shared storage to child process");
        exit(EXIT_FAILURE);
    }
    Storage* storage = (Storage *) shmMem;
    storage_set(storage);

    connect_handle(cfd, storageSem);

    storage_unset();
    shmdt(storage);

    puts("Prozess beendet");
    exit(EXIT_SUCCESS);
}

Connections connections;

void term_connection_controlling(int signal) {
    char* massage = "Server shutdown\n";
    int cfd;
    int pid;

    for (int i = 0; i < connections.len; i++) {
        cfd = connections.list[i].cfd;
        pid = connections.list[i].pid;
        send(cfd, massage, strlen(massage), 0);
        shutdown(cfd, SHUT_RDWR);
        close(cfd);

        kill(SIGTERM, pid);

        printf("Terminated process %i and closed socket %i\n", pid, cfd);
    }
    connections_delete(&connections);

    exit(0);
}

void child_finished(int signal, siginfo_t* info, void* context) {
    for (int i = 0; i < connections.len; i++) {
        if(connections.list[i].pid == info->si_pid) {
            while(i < connections.len) {
                connections.list[i].pid = connections.list[i+1].pid;
                connections.list[i].cfd = connections.list[i+1].cfd;
                i++;
            }
            connections.len--;

            return;
        }
    }
}

void connection_controlling(Config* config) {
    struct sockaddr_in client; // Socket Adresse eines Clients
    socklen_t client_len = sizeof(client); // Länge der Client-Daten
    connections = connections_new(2);

    struct sigaction sigterm;
    sigterm.sa_handler = term_connection_controlling;
    sigterm.sa_flags = 0;
    sigaction(SIGTERM, &sigterm, NULL);

    struct sigaction childFinished;
    childFinished.sa_flags = SA_NOCLDWAIT | SA_SIGINFO | SA_NOCLDSTOP;
    childFinished.sa_sigaction = child_finished;
    sigaction(SIGCHLD, &childFinished, NULL);


    // Verbindung eines Clients wird entgegengenommen
    while (1) {
        int cfd = accept(config->serverFd, (struct sockaddr *) &client, &client_len);
        if (cfd == -1) {
            continue;
        }

        if(connections.len < connections.cap) {
            int pid = fork();
            if(pid == 0)
                new_process(cfd, config->storageId, config->storageSem);
            else {
                connections_push(&connections, connection_new(pid, cfd));
            }
        }
        else {
            char* massage = "No place for new connection\n";
            send(cfd, massage, strlen(massage), 0);
            shutdown(cfd, SHUT_RDWR);
            close(cfd);
        }
    }
}

void run(Config* config) {
    char in[8];

    int connection_controller = fork();

    if(connection_controller == 0)
        connection_controlling(config);
    else {
        while(1) {
            scanf("%s", in);
            if(strcmp(in, "shutdown") == 0)
                break;
        }

        kill(connection_controller, SIGTERM);

        return;
    }
}