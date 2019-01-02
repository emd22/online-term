#ifndef CLIENT_HPP
#define CLIENT_HPP

#define DAT_SIZE 1024

#if defined(__linux__) || defined(__APPLE__) && !defined(UNIX)
#define UNIX
#include <netinet/in.h>
#elif defined(__WIN32) && !defined(WIN)
#define WIN
#endif

void client_init(int port, const char *ip);
void client_listen(void (*on_data_get)(char *));
char *client_read(char *dat);
void client_send(const char *dat);
void client_destroy(void);
void client_kill(void);
void client_set_kill_callback(void (*_kill_callback)(void));

#endif