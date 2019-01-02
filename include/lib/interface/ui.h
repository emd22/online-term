#ifndef JENKINS_UI_H
#define JENKINS_UI_H

#include <stdio.h>

#define CARET_SET_X(x) printf("\033[;%dH", x)
#define CARET_SET_LOCATION(x, y) printf("\033[%d;%dH", y, x)
#define CARET_MOVE_UP(amt)       printf("\033[%dA", amt)
#define CARET_MOVE_DOWN(amt)     printf("\033[%dB", amt)
#define CARET_MOVE_RIGHT(amt)    printf("\033[%dC", amt)
#define CARET_MOVE_LEFT(amt)     printf("\033[%dD", amt)

#define CLEAR_SCREEN   printf("\033[2J")

#define SAVE_SCREEN    printf("\033[?1049h")
#define RESTORE_SCREEN printf("\033[?1049l")

#define COLOUR_BLACK   0
#define COLOUR_RED     1
#define COLOUR_GREEN   2
#define COLOUR_YELLOW  3
#define COLOUR_BLUE    4
#define COLOUR_MAGENTA 5
#define COLOUR_CYAN    6
#define COLOUR_WHITE   7

#define ATTRIB_NONE 0
#define ATTRIB_BOLD 1

#define UI_SPACED 0x01

#define ERASE_LINE printf("\033[K");

#define SET_COLOUR(fg, bg) printf("\033[%d;%dm", fg+30, bg+40)
#define COLOUR_DEFAULT SET_COLOUR(COLOUR_WHITE, COLOUR_BLACK)

void ui_draw_one(char ch, int x, int y);
void ui_message(char *str, int x, int y, int flags);
void scroll_message(char *message, int window_sz, int trail);
void ui_init(void (*_resize_callback)(int, int));
void ui_destroy();

#endif