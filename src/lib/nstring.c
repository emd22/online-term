#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <nstring.h>

void string_new(string_t *string) {
    #ifdef DEBUG_3
    printf("string has been init'd.\n");
    #endif
    string->string = (char *)malloc(STRING_START_SIZE);
    string->size = STRING_START_SIZE;
    string->used = 0;
}

void string_edit(string_t *string, int index, char c) {
    if (string->used == string->size-1) {
        #ifdef DEBUG_3
        printf("reallocated string from %d bytes to %d at %p\n", string->size, string->size*2, &string->string);
        #endif
        string->string = realloc(string->string, string->size*2);
        string->size *= 2;
    }
    if (index > string->size || index < 0) {
        printf("trying to access string outside boundaries %d is not between %d and 0.\n", index, string->size);
        return;
    }
    string->string[index] = c;
    string->used++;
    if (index == string->used) {
        string->string[index+1] = 0;
        string->used++;
    }
}

char string_get(const string_t *string, int index) {
    if (index > string->size || index < 0) {
        printf("trying to access string outside boundaries. %d is not between %d and 0.\n", index, string->size);
        return 0;
    }
    return string->string[index];
}

void string_copy(string_t *dest, const string_t *src) {
    int i;
    int len = string_len(src);
    for (i = 0; i < len; i++) {
        string_edit(dest, i, string_get(src, i));
    }
}

void string_cat_c(string_t *dest, const char *src) {
    printf("string_cat_c in: %s\n", src);

    int i;
    int len = strlen(src);
    for (i = 0; i < len; i++) {
        string_edit(dest, string_len(dest), src[i]);
    }
}

void string_free(string_t *string) {
    #ifdef DEBUG_3
    printf("string(size: %d) has been freed.\n", string->size);
    #endif
    free(string->string);
}