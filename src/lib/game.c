#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <game.h>
#include <net/net.h>
#include <net/packet.h>
#include <interface/ui.h>
#include <interface/keyboard.h>

char *client_data;
pthread_t net_thread;
bool initialized = false;

typedef struct {
    int x;
    int y;
    int id;
} player_t;

// int term_width = 0, term_height = 0;

// void term_size_update(int width, int height) {
//     term_width = width;
//     term_height = height;
// }

void on_data_get(char *data) {
    if (data[0] == 0x02) {
        if (data[1] == CLIENT_LEFT) {
            char left_pack[] = NET_LEFT_PACKET(data[2]);
            server_broadcast(left_pack, data[2]);
        }
        else if (data[1] == CLIENT_MOVE)
            printf("lets move\n");
    }
    strcpy(client_data, data);
}

void *net_listener(void *arg) {
    (void)arg;
    client_listen(on_data_get);
    return NULL;
}

void game_end(void) {
    keyboard_set_flags(0);
    ui_destroy();
    free(client_data);
    client_kill();
    exit(0);
}

void char_set(int x, int y, char ch) {
    CARET_SET_LOCATION(x, y);
    printf("%c", ch);
}

void game_start(void) {
    client_data = malloc(128);
    int ret;

    ret = pthread_create(&net_thread, NULL, net_listener, NULL);
    if (ret) {
        fprintf(stderr, "Error: pthread_create returned %d\n", ret);
        exit(1);
    }

    ui_init(NULL);
    CLEAR_SCREEN;
    keyboard_set_flags(KBD_NONBLOCKING | KBD_NOECHO);

    initialized = true;
    
    // if we recieve SIGINT, call game_end to exit.
    client_set_kill_callback(game_end);

    int x = 1, y = 1;

    char_set(x, y, 'X');

    while (1) {
        switch (keyboard_getch()) {
            case 'w':
                char_set(x, y, ' ');
                char_set(x, --y, 'X');
                break;
            case 'a':
                char_set(x, y, ' ');
                char_set(--x, y, 'X');
                break;
            case 's':
                char_set(x, y, ' ');
                char_set(x, ++y, 'X');
                break;
            case 'd':
                char_set(x, y, ' ');
                char_set(++x, y, 'X');
                break;
            default:
                printf("unknown key\n");
        };
    }
}