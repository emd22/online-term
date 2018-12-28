#include <net/server.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

int server_fd, new_sockfd, cli_len;
struct sockaddr_in addr;
int client_fds[MAX_CLIENTS];
int client_index = 0;

bool end = false;

void server_error(const char *message) {
    printf("Server error: %s --- %s\n", message, strerror(errno));
    exit(1);
}

void serv_sig(int sig) {
    if (sig == SIGINT) {
        end = true;
    }
}

void server_init(int port, const char *ip) {
    memset(client_fds, 0, MAX_CLIENTS);
    signal(SIGINT, serv_sig);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        server_error("Error opening socket");
    }

    memset((char *)&addr, 0, sizeof(addr));

    in_addr_t ipaddr = inet_addr(ip);

    if (ipaddr == INADDR_NONE) {
        server_error("Error with ipaddr");
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ipaddr;
    addr.sin_port = htons(port);

    fcntl(server_fd, F_SETFL, O_NONBLOCK);    
    int ret = bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));

    if (ret < 0) {
        server_error("Error with binding. are you using the right network adapter?");
    }
}

void server_listen(void (*on_connect)(int, int), void (*client_left)(int *)) {
    listen(server_fd, 5);
    struct sockaddr_in cli_addr;
    char buf[128];

    cli_len = sizeof(cli_addr);

    while (true) {
        // check to see if there are clients to connect to
        new_sockfd = accept(server_fd, (struct sockaddr *)&cli_addr, (socklen_t *)&cli_len);

        // sleep for 100ms
        usleep(100);

        if (server_read_last(buf) != NULL) {
            switch (*buf) {
                case 0x04:
                    client_left(&client_index);
                    break;
                default:
                    printf("msg = %s\n", buf);
                    break;
            }
        }

        // not a successful socket run, check again.
        if (new_sockfd < 0) {
            if (end)
                return;
            continue;
        }

        // add client to client list
        client_fds[client_index++] = new_sockfd;
        // successful connection!
        on_connect(new_sockfd, client_index);
    }
}

char *server_read(int fd, char *dat) {
    int rsize = 0;

    memset(dat, 0, 128);

    rsize = recv(fd, dat, 128, 0);
    // if (rsize == -1) {
    //     server_error("Error reading from client");
    //     return NULL;
    // }
    if (rsize <= 0)
        return NULL;

    return dat;
}

void server_send(int fd, char *message) {
    int ret = 0;
    ret = send(fd, message, strlen(message), 0);
    if (ret < 0) {
        server_error("Error sending to client");
    }
    if (ret == 0) {
        int pos = 0;
        for (; pos < client_index; pos++) {
            if (client_fds[pos] == fd) {
                // vector_remove(&client_fds, pos);
                //TODO: FIX 
                printf("Client %d left.\n", pos);
                break;
            }
        }
    }
}

void server_broadcast(char *message) {
    for (int i = 0; i < client_index; i++)
        server_send(client_fds[i], message);
}

void server_kick(int fd, char *msg) {
    server_send(fd, msg);
    close(fd);
}

char *server_read_last(char *buf) {
    int i;
    memset(buf, 0, 128);
    for (i = 0; i < client_index; i++) {
        if (server_read(client_fds[i], buf) != NULL) {
            return buf;
        }
    }
    return NULL;
}

void server_destroy(void) {
    return;
}
