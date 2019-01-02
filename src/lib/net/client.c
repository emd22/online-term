#include <net/client.h>
#include <net/packet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <stdbool.h>
#include <pthread.h>

#define PACK_LEAVING 0x04

struct {
    int _index;
    int fd;
} client_info;

char server_ip[32];
int port;
struct sockaddr_in server, server_addr;
int server_fd;
void (*kill_callback)(void) = NULL;
bool initd = false;

bool end;

int client_get_index(void) {
    return client_info._index;
}

void client_error(const char *message) {
    printf("Client error: %s --- %s\n", message, strerror(errno));
    exit(1);
}

void client_set_kill_callback(void (*_kill_callback)(void)) {
    kill_callback = _kill_callback;
}

void client_kill(void) {
    if (!initd) {
        printf("Warning: client_kill trying to exit but is not initialized.\n");
        return;
    }
    if (client_info._index != -1) {
        char out_pack[128];
        char packet[] = NET_LEFT_PACKET(client_info._index);
        memcpy(out_pack, packet, 4);
        client_send(out_pack);
    }
    else
        printf("Warning: Client index == -1\n");
    end = 1;
    initd = false;
    // pthread_exit(NULL);
    return;
}

void cli_sig(int sig) {
    if (sig == SIGINT) {
        if (kill_callback != NULL)
            kill_callback();
        else {
            printf("Warning: Client exit callback == NULL\n");
            client_kill();
        }
    }
}

void client_init(int port_, const char *ip) {
    signal(SIGINT, cli_sig);

    client_info.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_info.fd < 0) {
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

    if (connect(client_info.fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        client_error("Error connecting");
    }
    initd = true;
}

void client_listen(void (*on_data_get)(char *)) {
    char *buf = (char *)malloc(128);

    while (true) {
        if (client_read(buf, 128) != NULL) {

            if (buf[0] == 0x02 && buf[1] == CLIENT_META) {
                // setup client metadata sent from server
                client_info._index = (int)(buf[2]);
            }

            else
                on_data_get(buf);
        }
        if (end)
            break;
    }

    free(buf);
    pthread_exit(NULL);
}

char *client_read(char *dat, int size) {
    int rsize = 0;

    memset(dat, 0, size);

    rsize = recv(client_info.fd, dat, size, 0);
    
    if (rsize == -1) {
        client_error("Error reading from server");
        return NULL;
    }
    if (rsize == 0)
        return NULL;
    return dat;
}

void client_send(const char *dat) {
    // int ret = 0;

    if (send(client_info.fd, dat, strlen(dat), 0) < 0) {
        client_error("Error sending from client");
    }
}

void client_destroy(void) {
    return;
}