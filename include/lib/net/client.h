#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <nstring.h>
#include <vector.h>

#define DAT_SIZE 1024

#if defined(__linux__) || defined(__APPLE__) && !defined(UNIX)
#define UNIX
#elif defined(__WIN32) && !defined(WIN)
#define WIN
#endif

#ifdef UNIX
#include <netinet/in.h>
#endif

void client_init(int port, const string_t *ip);
void client_listen(void *(*on_data_get)(const char *));
char *client_read(char *dat);
void client_send(const char *dat);
void client_destroy(void);

#endif