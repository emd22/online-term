#ifndef SERVER_HPP
#define SERVER_HPP

#include <nstring.h>
#include <vector.h>

#include <netinet/in.h>

#define DAT_SIZE 1024

typedef struct {
    int fd;
    int index;
    string_t dat;
} data_t;

void server_init();
void server_listen(void *(*on_connect)(int));
char *server_read(int fd, char *dat);
void server_send(int fd, const string_t *message);
void server_broadcast(const string_t *message);
void server_kick(int index, const string_t *msg);
vector_t /* std::vector<Data> */ server_read_all();
void server_destroy_readings(vector_t *dat_vec);
void server_destroy(void);

#endif