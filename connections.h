#ifndef PRAKBS22_CONNECTIONS_H
#define PRAKBS22_CONNECTIONS_H

typedef struct Connection {
    int pid;
    int cfd;
} Connection;

void connection_copy(Connection*, Connection*);
Connection connection_new(int, int);

typedef struct Connections {
    unsigned int cap;
    unsigned int len;
    Connection* list;
} Connections;

Connections connections_new(int);
void connections_delete(Connections*);
void connections_push(Connections*, Connection);

#endif //PRAKBS22_CONNECTIONS_H
