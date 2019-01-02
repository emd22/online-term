#include <interface/keyboard.h>

#include <termios.h>
#include <stdio.h>

// struct termios tinf_old, tinf_new;

void keyboard_set_flags(int flags) {
    struct termios tinf;
    tcgetattr(1, &tinf);

    if (flags & KBD_NONBLOCKING)
        tinf.c_lflag &= ~ICANON;
    else
        tinf.c_lflag |= ICANON;

    if (flags & KBD_NOECHO)
        tinf.c_lflag &= ~ECHO;
    else
        tinf.c_lflag |= ECHO;

    // tinf.c_lflag &= (flags & KBD_NONBLOCKING) ? ICANON : ~ICANON;
    // tinf.c_lflag &= ((flags & KBD_NOECHO) ? ~ECHO : ECHO);

    tcsetattr(1, 0, &tinf);
}

char keyboard_getch(void) {
    return getchar();
}