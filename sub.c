#include "sub.h"

int sonderzeichen(char *string){
    for(int i = 0; i < strlen(string); i++) {
        if (!((int) string[i] >= 48 && (int) string[i] <= 57 || (int) string[i] >= 65 && (int) string[i] <= 90 || (int) string[i] >= 97 && (int) string[i] <= 122 || (int) string[i] == 32) ) //ASCII: 48-57, 65-90, 97-122, 32
            return 0;
    }
    return 1;
}

int connect_handle(int conn_fd) {
    char in[BUFFSIZE], out[BUFFSIZE];

    while(1) {
        bzero(in, BUFFSIZE);
        bzero(out, BUFFSIZE);

        // Receive Command from client
        if(recv(conn_fd, in , BUFFSIZE, 0) == -1) {
            perror("ERROR: Can't receive massage from client");
            return -1;
        }

        // Interpret massage from client
        char* command = strtok(in, " ");

        if(strcmp(command, "PUT") == 0) {
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
                if (put(key, value) != 0) {
                    strcat(out, ":");
                    strcat(out, value);
                } else {
                    strcat(out, ":no_space_for_new_key");
                }
            }
        }
        else if(strncmp(command, "GET", 3) == 0) {
            char* key= strtok(NULL, "\r");

            if(key == NULL) {
                strcpy(out, "command_nonexistent");
            }
            else {
                char value[BUFFSIZE];

                strcpy(out, "GET :");
                strcat(out, key);
                if (get(key, value) == 1) {
                    strcat(out, ":");
                    strcat(out, value);
                    puts(value);
                } else {
                    strcat(out, ":key_nonexistent");
                }
            }
        }
        else if(strncmp(command, "DEL", 3) == 0){
            char* key= strtok(NULL, "\r");
            if(key== NULL) {
                strcpy(out, "command_nonexistent");
            }
            else{
                strcpy(out, "DEL :");
                strcat(out, key);
                if (del(key) == 1) {
                    strcat(out, ":key_deleted");
                } else {
                    strcat(out, ":key_nonexistent");
                }
            }
        }
        else if(strncmp(in, "QUIT\r", 5) == 0) {
            shutdown(conn_fd, SHUT_RDWR);
            return 1;
        }
        else
            strcpy(out,"command_nonexistent");

        // New line after output
        strcat(out, "\n");

        send(conn_fd, out, BUFFSIZE, 0);
    }
}