#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KBD_NOECHO      0x01
#define KBD_NONBLOCKING 0x02

void keyboard_set_flags(int flags);
char keyboard_getch(void);

#endif