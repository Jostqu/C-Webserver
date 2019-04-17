//
// Created by bm502101 on 17.04.19.
//

#include "string.h"

void string_concat(string *str, char *src) {
    int len = strlen(src);

    if(str->len + len > str->capacity){
        str->capacity = ((str->len + len) *2);
        str->buf = (char*)realloc(str->buf, str->capacity);
    }

    if(!str->buf){
        perror("failed to realloc string->buffer");
        exit(1);
    }

    memcpy((str->buf+str->len), src, len);
    str->len += len;
}

string* string_new(size_t capacity)
{
    string* str = malloc(sizeof(string));
    if (str == NULL)
    {
        error("string_new: failed to allocate string!");
    }

    str->buf = malloc(capacity);
    if (str->buf == NULL)
    {
        error("string_new: failed to allocate string buffer!");
    }

    str->capacity = capacity;
    str->len = 0;
    return str;
}

string* string_copy(string* str)
{
    string* copy = string_new(str->len * 2);
    memcpy(copy->buf, str->buf, str->len);
    copy->len = str->len;
    return copy;
}

void string_free(string* str)
{
    free(str->buf);
    free(str);
}

