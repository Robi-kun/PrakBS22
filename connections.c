#include <malloc.h>
#include "connections.h"



void connection_init(Connection* this, int pid, int cfd) {
    this->pid = pid;
    this->cfd = cfd;
}

void connection_copy(Connection* dest, Connection* src) {
    dest->pid = src->pid;
    dest->cfd = src->cfd;
}

Connection connection_new(int pid, int fd) {
    Connection connection;

    connection_init(&connection, pid, fd);

    return connection;
}



void connections_init(Connections* this, int cap) {
    this->cap = cap;
    this->len = 0;
    this->list = calloc(cap, sizeof(Connection));
}

Connections connections_new(int cap) {
    Connections connections;

    connections_init(&connections, cap);

    return connections;
}

void connections_delete(Connections* this) {
    free(this->list);
}

void connections_push(Connections* this, Connection new) {
    unsigned int i = this->len;

    connection_copy(&this->list[i], &new);
    this->len++;
}
