//
// Created by bm502101 on 17.04.19.
//

#include "string.h"

string* string_new(size_t capacity)
{
    string* str = malloc(sizeof(string));
    if (str == NULL)
    {
        perror("string_new: failed to allocate string!");
        exit(1);
    }

    str->buf = malloc(capacity);
    if (str->buf == NULL)
    {
        perror("string_new: failed to allocate string buffer!");
        exit(2);
    }

    str->capacity = capacity;
    str->len = 0;
    return str;
}

string* string_copy(string* str)
{
    // Neuen String erstellen und gesamten Inhalt von str hineinkopieren
    string* copy = string_new(str->len * 2);
    memcpy(copy->buf, str->buf, str->len);
    copy->len = str->len;
    return copy;
}

void string_free(string* str)
{
    // Puffer und Struct freigeben
    free(str->buf);
    free(str);
}

void string_concat(string *str, char *src) {
    int len = strlen(src);

    if(!str || !src){
        perror("str or src is null");
        exit(2);
    }

    if(str->len + len > str->capacity){
        str->capacity = ((str->len + len) * 2);
        str->buf = (char*)realloc(str->buf, str->capacity);
    }

    if(!str->buf){
        perror("failed to realloc string->buffer");
        exit(1);
    }

    memcpy((str->buf+str->len), src, len);
    str->len += len;
}

void string_add_char(string* str, char c)
{
    if (str->capacity < str->len + 1)
    {
        str->capacity = (str->len + 1) * 2;
        str->buf = realloc(str->buf, str->capacity);
        if (str->buf == NULL)
        {
            perror("string_add_char: failed to reallocate string buffer!");
        }
    }

    str->buf[str->len++] = c;
}

string* string_strip(string* str)
{
    // leere Zeichen am Anfang entfernen
    for (size_t i = 0; i < str->len; i++)
    {
        // Aktuelles Zeichen
        char c = str->buf[i];

        // Wenn kein leeres Zeichen
        if (!(c == ' ' || c == '\n' || c == '\r'))
        {
            // Wenn nicht am Anfang
            if (i > 0)
            {
                // String um i nach links verschieben
                memmove(str->buf, str->buf + i, str->len - i);

                // Neue Länge setzen
                str->len -= i;
            }
            break;
        }
    }

    // leere Zeichen am Ende entfernen
    for (size_t i = str->len - 1; i >= 0; i--)
    {
        // Aktuelles Zeichen
        char c = str->buf[i];

        // Wenn kein leeres Zeichen
        if (!(c == ' ' || c == '\n' || c == '\r'))
        {
            // Wenn nicht am Ende
            if (i < str->len - 1)
            {
                // Neue Länge setzen
                str->len = i + 1;
            }
            break;
        }
    }

    return str;
}

bool string_compare(string* string1, string* string2){

    if(string1->len != string2->len) //sanity check
        return false;

    if(memcmp(string1->buf, string2->buf, string1->len) == 0)
        return true;
    else
        return false;
}

void string_print(string* str)
{
    // Jedes Zeichen auf der Konsole ausgeben
    for (size_t i = 0; i < str->len; i++)
    {
        putchar(str->buf[i]);
    }
    putchar('\n');
}

void string_concat_str(string *dst, string *src) {

    if((dst->len + src->len) > dst->capacity){

        dst->capacity = (dst->len + src->len)*2;
        dst->buf = realloc(dst->buf, dst->capacity);
    }

    if(!dst->buf){
        perror("failed to realloc");
        exit(3);
    }

    memcpy((dst->buf + dst->len), src->buf, src->len);
    dst->len += src->len;
}

string* int_to_string(int i) {

    string* str = string_new(12);
    char number[12];

    sprintf(number, "%d", i);

    string_concat(str, number);

    return str;
}

string **string_split(string *str, char splitter, int* splits) {
    if(!str)
        return NULL;

    *splits = 0;

    //count splitters
    for (int i = 0; i < str->len; i++) {
        if(str->buf[i] == splitter)
            (*splits)++;
    }

    string** list = calloc(*splits, sizeof(struct string_struct));

    if(!list)
        return NULL;

    list[0] = string_new(10);

    int i = 0;
    for (int j = 0; j < str->len; j++) {
        if (str->buf[j] != splitter) {
            if(list[i])
                string_add_char(list[i], str->buf[j]);
            else
                return NULL;
        }else {
            i++;
            list[i] = string_new(10);
        }
    }

    return list;
}

string *string_terminate(string *str)
{
	if (str->len + 1 > str->capacity)
	{
		str->capacity = (str->len + 1) * 2;
		str->buf = realloc(str->buf, str->capacity);
		if (!str->buf)
		{
			perror("string_terminate: failed to reallocate string buffer!");
			exit(1);
		}
	}

	str->buf[str->len] = '\0';
	return str;
}

long string_to_long(string *str, int base) {
    string_terminate(str); //to be sure
    return strtol(str->buf, NULL, base);
}
