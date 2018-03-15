#ifndef STRING_H
#define STRING_H

#define STRING_START_SIZE 64

#define DEBUG_3

typedef struct {
    char *string;
    int size;
    int used;
} string_t;

#define string_len(string) string->used

void string_new(string_t *string);
void string_edit(string_t *string, int index, char c);
char string_get(const string_t *string, int index);
void string_copy(string_t *dest, const string_t *src);
void string_free(string_t *string);
void string_cat_c(string_t *dest, const char *src);

#endif