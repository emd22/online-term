#ifndef NET_HPP
#define NET_HPP

#define NET_DEFAULT_PORT 8088

#include <net/server.h>
#include <net/client.h>

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

char *net_get_default_device(char *dev);
char *net_get_ip(addr_t *addr, char *ipstr);
char *net_get_host_ip(char *adapter);

#endif