#include <net/server.h>
#include <net/packet.h>

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

client_t clients[MAX_CLIENTS];

bool end = false;

void generate_random_id(client_t *client) {
    int i;
    for (i = 0; i < 8; i++) {
        client->id[i] = rand()%(90-65+1)+65;
    }
}

void server_error(const char *message) {
    printf("Server error: %s --- %s\n", message, strerror(errno));
    exit(1);
}

void serv_sig(int sig) {
    if (sig == SIGINT) {
        end = true;
    }
}

client_t *find_free_client(int *index) {
    int i;
    client_t *this_client;
    for (i = 0; i < MAX_CLIENTS; i++) {
        this_client = &clients[i];


        if (!this_client->used) {
            (*index) = i;
            return this_client;
        }
    }
    return NULL;
}

void server_init(int port, const char *ip) {
    memset(clients, 0, sizeof(client_t)*MAX_CLIENTS);
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

void server_listen(void (*on_connect)(int, int), void (*client_left)(char), void (*on_data_get)(int, char *)) {
    listen(server_fd, 5);
    struct sockaddr_in cli_addr;
    char buf[128];
    int sender;

    cli_len = sizeof(cli_addr);

    while (true) {
        // check to see if there are clients to connect to
        new_sockfd = accept(server_fd, (struct sockaddr *)&cli_addr, (socklen_t *)&cli_len);

        // sleep for 100ms
        usleep(100);

        if (server_read_last(buf, &sender) != NULL) {
            if (buf[0] == 0x02 && buf[1] == CLIENT_LEFT) {
                printf("client %d left.\n", buf[2]);
                clients[(int)(buf[2])].used = 0;
            }
            on_data_get(sender, buf);
        }

        // not a successful socket run, check again.
        if (new_sockfd < 0) {
            if (end)
                return;
            continue;
        }

        // add client to client list
        int client_index;
        client_t *new_client = find_free_client(&client_index);
        if (new_client == NULL) {
            printf("Cannot accept client(Lobby full)\n");
            continue;
        }
        new_client->fd = new_sockfd;
        generate_random_id(new_client);
        new_client->used = true;
        char meta[] = NET_SEND_CLIENT_META(client_index);
        server_send(new_sockfd, meta);
        // successful connection!
        on_connect(new_sockfd, client_index);
    }
}

char *server_read(int fd, char *dat) {
    int rsize = 0;

    memset(dat, 0, 128);

    rsize = recv(fd, dat, 128, 0);
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
}

void server_broadcast(char *message, int sender) {
    client_t *this_client;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        this_client = &clients[i];
        if (!this_client->used || i == sender)
            continue;
        server_send(this_client->fd, message);
    }
}

void server_kick(int fd, char *msg) {
    server_send(fd, msg);
    close(fd);
}

char *server_read_last(char *buf, int *sender) {
    int i;
    memset(buf, 0, 128);
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (server_read(clients[i].fd, buf) != NULL) {
            (*sender) = i;
            printf("%d sent us (%s)\n", i, buf);
            return buf;
        }
    }
    return NULL;
}

void server_destroy(void) {
    return;
}
