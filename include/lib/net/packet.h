#ifndef PACKET_H
#define PACKET_H

enum {
    CLIENT_LEFT,
    CLIENT_CONNECTED,
    CLIENT_MOVE,
    CLIENT_META,
};

#define NET_LEFT_PACKET(client_index) {0x02, CLIENT_LEFT, client_index}
#define NET_SEND_CLIENT_META(index) {0x02, CLIENT_META, index}
// #define NET_CLIENT_MOVE(client_index, x, y) {0x02, }

#endif