#include "main.h"

#define BUFSIZE 1024 // Größe des Buffers
#define TRUE 1
#define ENDLOSSCHLEIFE 1
//#define PORT 4711
#define PORT 5678
#define STRINGSIZE 100

int main() {

    arrayEinrichten();

    int rfd; // Rendevouz-Descriptor
    int cfd; // Verbindungs-Descriptor

    struct sockaddr_in client; // Socketadresse eines Clients
    socklen_t client_len; // Länge der Client-Daten
    char in[BUFSIZE]; // Daten vom Client an den Server
    int bytes_read; // Anzahl der Bytes, die der Client geschickt hat


    // Socket erstellen
    rfd = socket(AF_INET, SOCK_STREAM, 0);
    if (rfd < 0 ){
        fprintf(stderr, "socket konnte nicht erstellt werden\n");
        exit(-1);
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
        exit(-1);
    }


    // Socket lauschen lassen
    int lrt = listen(rfd, 5);
    if (lrt < 0 ){
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(-1);
    }

    while (ENDLOSSCHLEIFE) {

        // Verbindung eines Clients wird entgegengenommen
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);

        // Lesen von Daten, die der Client schickt
        bytes_read = read(cfd, in, BUFSIZE);


        // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
        while (bytes_read > 0) {
            printf("sending back the %d bytes I received...\n", bytes_read);


            //Interpretation
            int a;
            char x[STRINGSIZE];
            char* command = strtok(in, " ");
            if(strcmp(command, "PUT") == 0) {
                char *keyptr = strtok(NULL, " ");
                char *valptr = strtok(NULL, "\r");
                puts(keyptr);
                puts(valptr);
                if(keyptr == NULL || valptr == NULL) {
                    write(cfd, "command_nonexistent\n", 20);
                }
                else if(sonderzeichen(keyptr) == 0 || sonderzeichen(valptr) == 0){
                    write(cfd,"special_characters_not_allowed\n",31);
                }
                else {
                    printf("Key: %s\n", keyptr);
                    printf("Value: %s\n", valptr);

                    a = put(keyptr, valptr);

                    char y[STRINGSIZE+STRINGSIZE+10] = "PUT :";
                    strcat(y, keyptr);
                    if (a == 1 || a == -1) {
                        strcat(y, ":");
                        strcat(y, valptr);
                        strcat(y, "\n");
                    } else {
                        strcat(y, ":no_space_for_new_key\n");
                    }
                    write(cfd, y, strlen(y));
                }
            }

            else if(strcmp(command, "GET") == 0) {
                char *keyptr = strtok(NULL, "\r");

                if(keyptr == NULL) {
                    write(cfd, "command_nonexistent\n", 20);
                }
                else {
                    a = get(keyptr, x);

                    char y[STRINGSIZE+STRINGSIZE+10] = "GET :";
                    strcat(y, keyptr);
                    if (a == 1) {
                        strcat(y, ":");
                        strcat(y, x);
                        strcat(y, "\n");
                    } else {
                        strcat(y, ":key_nonexistent\n");
                    }
                    write(cfd, y, strlen(y));
                }
            }

            else if(strcmp(command, "DEL") == 0){
                char *keyptr = strtok(NULL, "\r");
                if(keyptr == NULL) {
                    write(cfd, "command_nonexistent\n", 20);
                }
                else{
                    a = del(keyptr);

                    char y[STRINGSIZE+50] = "DEL :";
                    strcat(y, keyptr);
                    if (a == 1) {
                        strcat(y, ":key_deleted\n");
                    } else {
                        strcat(y, ":key_nonexistent\n");
                    }
                    write(cfd, y, strlen(y));
                }
            }
            else if(strncmp(in, "QUIT\r", 5) == 0)
                close(cfd);

            else
                write(cfd,"command_nonexistent\n",20);
            //Interpretation Ende


            bytes_read = read(cfd, in, BUFSIZE);
        }
        close(cfd);
    }

    // Rendevouz Descriptor schließen
    close(rfd);

}