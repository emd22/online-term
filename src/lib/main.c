#include <net/net.h>
#include <game.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>

#define ISNUM(ch) (ch >= '0' && ch <= '9')

bool client;
int port;
char ip[32];

void on_connect(int client_fd, int client_index) {
    printf("client %d has connected.\n", client_index);
    // server_send(client_fd, "Hello, new client!");
}

void client_left(char client_index) {
    printf("client %d left.\n", client_index);
    // server_broadcast("a client has left", BROADCAST_ALL);
}

void server_on_data_get(int sender, char *data) {
    server_broadcast(data, sender);
}

void server_start() {
    printf("Join this server using port %d and ip %s!\n", port, ip);

    server_init(port, ip);
    server_listen(on_connect, client_left, server_on_data_get);
}

void client_start() {
    client_init(port, ip);
    // client_send("Hello, server.");
    game_start();
}

int main(int argc, char *argv[]) {
    client = false;
    
    int option, i;
    char *ipstr = NULL;

    port = NET_DEFAULT_PORT;

    while ((option = getopt(argc, argv, "hsci:p:")) != -1) {
        switch (option) {
            case 's': // Server option
                client = false;
                break;
            case 'c': // Client option
                client = true;
                break;
            case 'i': // Implicit set IP address
                ipstr = optarg;
                break;
            case 'p': // Implicit set port
                if (!strcmp(optarg, "default"))
                    break;
                port = atoi(optarg);
                for (i = 0; i < strlen(optarg); i++) {
                    if (!ISNUM(optarg[i])) {
                        port = NET_DEFAULT_PORT;
                        break;
                    }
                }
                break;
            case '?':
                if (optopt == 'i')
                    fprintf(stderr, "IP setting requires ip string:\n\t%s -i 127.0.0.1\n", argv[0]);
                else if (optopt == 'p')
                    fprintf(stderr, "Port setting requires port number:\n\t%s -p 8088\n", argv[0]);
                else
                    fprintf(stderr, "Unknown option '-%c'", optopt);
                return 1;

            case 'h':
                printf(
                    "Options:\n"
                    "   -s: start as server\n"
                    "   -c: start as client\n"
                    "   -i: set ip address\n"
                    "   -p: set port\n"
                );
                return 0;
            default:
                return 0;
        };
    }
    for (i = optind; i < argc; i++) {
        printf("Unknown argument %s\n", argv[i]);
    }

    if ((ipstr == NULL || !strcmp(ipstr, "default")) && client == true) {
        printf("Client connection requires ip argument.\n\t<prog name> -i 127.0.0.1\n");
        return 1;
    }

    if (ipstr == NULL || !strcmp(ipstr, "default")) {
        char def_dev[32];
        memset(def_dev, 0, 32);
        if (net_get_default_device(def_dev) == NULL) {
            printf("Cannot find default device, please set ip implictly.\n");
            return 1;
        }

        printf("Using default device [%s]\n", def_dev);
        ipstr = net_get_host_ip(def_dev);
    }

    memcpy(ip, ipstr, 32);

    if (client) {
        printf("Starting client...\n\n");
        client_start();
        client_destroy();
    }
    else {
        printf("Starting server...\n\nUse Ctrl+C to exit\n");
        server_start();
        server_destroy();
    }
}