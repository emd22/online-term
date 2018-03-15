#ifndef NET_HPP
#define NET_HPP

#define FAIL 1
#define SUCCESS 0

#define CLIENT true
#define SERVER false

#define IP_STR_LEN 32

#include <net/server.h>
#include <net/client.h>

#include <vector.h>
#include <string.h>

#if defined(__linux__) || defined(__APPLE__)
#define UNIX
#elif defined(__WIN32)
#define WIN
#endif

#ifdef UNIX
#include <netinet/in.h>

typedef struct sockaddr_in addr_t;

#endif

#define DAT_SIZE 1024

char *net_get_default_device(void);
string_t net_get_ip(addr_t *addr);
string_t net_get_host_ip(const char *adapter);

#endif