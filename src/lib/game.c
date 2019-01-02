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

pthread_t net_thread;
bool initialized = false;

typedef struct {
    int x;
    int y;
    int colour;
} player_t;

player_t players[8];

// int term_width = 0, term_height = 0;

// void term_size_update(int width, int height) {
//     term_width = width;
//     term_height = height;
// }

void char_set(int x, int y, char ch) {
    CARET_SET_LOCATION(x, y);
    printf("%c", ch);
    fflush(stdout);
}

void on_data_get(char *data) {
    if (data[0] == 0x02) {
        if (data[1] == CLIENT_LEFT) {
            char dat_pack[128];
            char left_pack[] = NET_LEFT_PACKET(data[2]);
            memcpy(dat_pack, left_pack, 4);
            server_broadcast(dat_pack, data[2]);
        }
        else if (data[1] == CLIENT_MOVE) {
            players[(int)(data[2])].x = data[3];
            players[(int)(data[2])].y = data[4];
            char_set((int)(data[3]), (int)(data[4]), 'O');
        }
    }
}

void *net_listener(void *arg) {
    (void)arg;
    client_listen(on_data_get);
    return NULL;
}

void game_end(void) {
    keyboard_set_flags(0);
    ui_destroy();
    client_kill();
    exit(0);
}

void player_update(player_t *player, char ch, int move_x, int move_y) {
    char_set(player->x, player->y, ch);
    (player->x) += move_x;
    (player->y) += move_y;
    char_set(player->x, player->y, ch);
    char out_pack[128];
    char packet[] = NET_CLIENT_MOVE(client_get_index(), player->x, player->y);
    memcpy(out_pack, packet, 6);
    client_send(out_pack);
}

void game_start(void) {
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

    char in_dat[128];

    player_t my_player;
    memset(&my_player, 0, sizeof(player_t));

    my_player.x = 1;
    my_player.y = 1;

    player_update(&my_player, 'X', 0, 0);

    while (1) {
        switch (keyboard_getch()) {
            case 'w':
                player_update(&my_player, 'X',  0, -1);
                break;
            case 'a':
                player_update(&my_player, 'X', -1,  0);
                break;
            case 's':
                player_update(&my_player, 'X',  0,  1);
                break;
            case 'd':
                player_update(&my_player, 'X',  1,  0);
                break;

            default:
                printf("unknown key\n");
        };
    }
}