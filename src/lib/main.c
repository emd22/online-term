#include <net/net.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool client;

string_t ip;
int port;

void arg_check(char *argv[]) {
    if (!strcmp(argv[1], "0")) {
        client = true;
    }
    else if (!strcmp(argv[1], "1")) {
        client = false;
    }
    else {
        exit(1);
    }
}

void *on_connect(int client_fd) {
    printf("client %d has connected.\n", client_fd);

    string_t out;
    string_new(&out);
    string_cat_c(&out, "Hello, new client!");
    server_broadcast(&out);
    string_free(&out);
}

void *on_data_get(const char *dat) {
    printf("recieved message '%s' from server\n", dat);
}

void server_start() {
    printf("Join this server using port %d and ip %s!\n", port, ip.string);

    server_init(port, &ip);
    server_listen(on_connect);
}

void client_start() {
    client_init(port, &ip);
    client_listen(on_data_get);
}

int main(int argc, char *argv[]) {
    client = false;
    
    if (argc != 2) {
        puts("Usage: <program-name> <0|1>");
        return 1;
    }
    arg_check(argv);

    string_new(&ip);

    string_t host_ip = net_get_host_ip(net_get_default_device());
    string_copy(&ip, &host_ip);
    string_free(&host_ip);

    port = 8088;

    if (client) {
        client_start();
        client_destroy();
    }
    else {
        server_start();
        server_destroy();
    }
    string_free(&ip);
}