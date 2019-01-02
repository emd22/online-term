#include <interface/ui.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

static void (*resize_callback)(int, int);
static int tsize[2];

void win_size(int *size) {
    struct winsize ts;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
    size[0] = ts.ws_col;
    size[1] = ts.ws_row;
}

void on_resize(int sig) {
    win_size(tsize);
    if (resize_callback != NULL)
        resize_callback(tsize[0], tsize[1]);
}

void ui_init(void (*_resize_callback)(int, int)) {
    SAVE_SCREEN;
    resize_callback = _resize_callback;
    win_size(tsize);
    signal(SIGWINCH, on_resize);
}

void ui_draw_one(char ch, int x, int y) {
    CARET_SET_LOCATION(x, y);
    printf("%c", ch);
    // return to original caret location
    CARET_MOVE_LEFT(1);
}

void ui_message(char *str, int x, int y, int flags) {
    CARET_SET_LOCATION(x, y);
    int strl = strlen(str);
    int i;
    for (i = 0; i < strl; i++) {
        printf("%c", str[i]);
        if (flags & UI_SPACED)
            CARET_MOVE_RIGHT(1);
    }
    fflush(stdout);
}

void ui_destroy() {
    RESTORE_SCREEN;
}