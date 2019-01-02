#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>

#define MAX_CLIENTS 8
#define BROADCAST_ALL -1

// typedef struct {
//     int fd;
//     int index;
//     char dat[128];
// } data_t;

typedef struct {
    char used;
    int fd;
    char id[8];
} client_t;

void server_init();
void server_listen(void (*on_connect)(int, int), void (*client_left)(char), void (*on_data_get)(int, char *));
void server_send(int fd, char *message);
void server_broadcast(char *message, int sender);
void server_kick(int fd, char *msg);
char *server_read_last(char *buf, int *sender);
void server_destroy(void);

#endif