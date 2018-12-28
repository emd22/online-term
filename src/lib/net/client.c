#include <net/client.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <stdbool.h>

#define PACK_LEAVING 0x04

int client_fd;
int server_fd;
struct sockaddr_in server, server_addr;
int port;
char server_ip[32];

static bool end;

void client_error(const char *message) {
    printf("Client error: %s --- %s\n", message, strerror(errno));
    exit(1);
}

void cli_sig(int sig) {
    if (sig == SIGINT) {
        char packet[16];
        packet[0] = PACK_LEAVING;
        packet[1] = 0;
        client_send(packet);
        exit(1);
    }
}

void client_init(int port_, const char *ip) {
    signal(SIGINT, cli_sig);

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        client_error("Error opening socket");
    }
    //init socket struct
    memset((char *)&server, 0, sizeof(server));

    in_addr_t ipaddr = inet_addr(ip);

    if (ipaddr == INADDR_NONE) {
        client_error("Error with ipaddr");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = ipaddr;
    server.sin_port = htons(port_);

    memcpy(server_ip, ip, 32);
    port = port_;
    end = false;

    // int ret = inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    // if (ret <= 0) {
    //     client_error("Error with inet_pton\n");
    // }

    if (connect(client_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        client_error("Error connecting");
    }
}

void client_listen(void *(*on_data_get)(const char *)) {
    char *buf = (char *)malloc(DAT_SIZE);

    while (true) {
        if (client_read(buf) != NULL) {
            on_data_get(buf);
        }
        if (end)
            break;
    }

    free(buf);
}

char *client_read(char *dat) {
    int rsize = 0;

    memset(dat, 0, 128);

    rsize = recv(client_fd, dat, 128, 0);
    
    if (rsize == -1) {
        client_error("Error reading from server");
        return NULL;
    }
    if (rsize == 0)
        return NULL;
    return dat;
}

void client_send(const char *dat) {
    int ret = 0;

    if (send(client_fd, dat, strlen(dat), 0) < 0) {
        client_error("Error sending from client");
    }
}

void client_destroy(void) {
    return;
}