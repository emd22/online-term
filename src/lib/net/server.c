#include <net/server.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <netdb.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>

#include <errno.h>

int server_fd, new_sockfd, cli_len;
struct sockaddr_in addr;
vector_t /* std::vector<int> */ client_fds;

bool end = false;

void server_error(const char *message) {
    printf("Client error: %s --- %s\n", message, strerror(errno));
    exit(1);
}

void serv_sig(int sig) {
    if (sig == SIGINT) {
        end = true;
    }
}

void server_init(int port, const string_t *ip) {
    signal(SIGINT, serv_sig);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        server_error("Error opening socket");
    }

    memset((char *)&addr, 0, sizeof(addr));

    in_addr_t ipaddr = inet_addr(ip->string);

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

    vector_init(&client_fds);
}

void server_listen(void *(*on_connect)(int)) {
    listen(server_fd, 5);
    struct sockaddr_in cli_addr;

    cli_len = sizeof(cli_addr);

    while (true) {
        new_sockfd = accept(server_fd, (struct sockaddr *)&cli_addr, (socklen_t *)&cli_len);

        while (errno == EWOULDBLOCK) {
            if (end)
                return;
        }

        if (new_sockfd < 0) {
            server_error("Error with accepting client");
        }
        vector_add(&client_fds, &new_sockfd);
        on_connect(new_sockfd);
    }
}

char *server_read(int fd, char *dat) {
    int rsize = 0;

    memset(dat, 0, 1024);

    rsize = recv(fd, dat, 1024, 0);
    if (rsize == -1) {
        server_error("Error reading from client");
        return NULL;
    }
    if (rsize == 0)
        return NULL;
    return dat;
}

void server_send(int fd, const string_t *message) {
    int ret = 0;

    ret = send(fd, message->string, message->used, 0);
    if (ret < 0) {
        server_error("Error sending to client");
    }
    if (ret == 0) {
        int pos = 0;
        int *vget = NULL;
        for (; pos < client_fds.used; pos++) {
            vget = (int *)vector_get(&client_fds, pos);
            if (*vget == fd) {
                vector_remove(&client_fds, pos);
                printf("Client %d left.\n", pos);
                break;
            }
        }
    }
}

void server_broadcast(const string_t *message) {
    int len = client_fds.used;
    int *vget = NULL;

    for (int i = 0; i < len; i++) {
        vget = (int *)vector_get(&client_fds, i);
        server_send(*vget, message);
    }
}

void server_kick(int fd, const string_t *msg) {
    server_send(fd, msg);
    close(fd);
}

vector_t server_read_all() {
    vector_t dat_vec;
    vector_init(&dat_vec);
    
    int len = client_fds.used;
    char *dat = (char *)malloc(DAT_SIZE);
    int *vget = NULL;

    for (int i = 0; i < len; i++) {
        vget = (int *)vector_get(&client_fds, i);
        if (server_read(*vget, dat) != NULL) {
            data_t data;
            string_cat_c(&data.dat, dat);
            data.fd = *vget;
            data.index = i;
            vector_add(&dat_vec, &data);
        }
    }
    
    free(dat);
    return dat_vec;
}

void server_destroy_readings(vector_t *dat_vec) {
    int i;
    for (i = 0; i < dat_vec->used; i++) {
        string_free(&((data_t *)vector_get(dat_vec, i))->dat);
    }
}

void server_destroy(void) {
    vector_free(&client_fds);
}
