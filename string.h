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

/**
 * \brief Entfernt leere Zeichen am Anfang und am Ende des Strings
 * @param str String
 * @return Selber String wie Argument
 */
string* string_strip(string* str);

/**
 * \brief Gibt einen String mit Absatz auf der Konsole aus
 * @param str String
 */
void string_print(string* str);

#endif //PSE_STRING_H
