#ifndef PACKET_H
#define PACKET_H

enum {
    CLIENT_LEFT,
    CLIENT_CONNECTED,
    CLIENT_MOVE,
    CLIENT_META,
};

#define NET_LEFT_PACKET(client_index) {0x02, CLIENT_LEFT, client_index, 0}
#define NET_SEND_CLIENT_META(index, amt_players) {0x02, CLIENT_META, index, amt_players, 0}
#define NET_CLIENT_MOVE(client_index, x, y) {0x02, CLIENT_MOVE, client_index, x, y, 0}

#endif