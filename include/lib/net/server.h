#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>

#define MAX_CLIENTS 8

typedef struct {
    int fd;
    int index;
    char dat[128];
} data_t;

void server_init();
void server_listen(void (*on_connect)(int, int), void (*client_left)(int *));
void server_send(int fd, char *message);
void server_broadcast(char *message);
void server_kick(int fd, char *msg);
char *server_read_last(char *buf);
void server_destroy(void);

#endif