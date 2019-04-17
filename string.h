//
// Created by bm502101 on 17.04.19.
//

#ifndef PSE_STRING_H
#define PSE_STRING_H

#include <malloc.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef struct string_struct
{
    size_t capacity;
    size_t len;
    char *buf;
} string;

string* string_new(size_t capacity);

string* string_copy(string* str);

void string_free(string* str);

void string_concat(string* str, char* src); //len removed by request of marsel

#endif //PSE_STRING_H
