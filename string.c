//
// Created by bm502101 on 17.04.19.
//

#include "string.h"

string* string_new(size_t capacity)
{
    string* str = calloc(1, sizeof(string));
    if (str == NULL)
    {
        perror("string_new: failed to allocate string!");
        exit(1);
    }

    str->buf = calloc(capacity, sizeof(char));
    if (str->buf == NULL)
    {
        perror("string_new: failed to allocate string buffer!");
        exit(2);
    }

    str->capacity = capacity;
    str->len = 0;
    return str;
}

void string_realloc(string* str, size_t newCap){
    if(str){
        str->buf = realloc(str->buf, newCap);
        str->capacity = newCap;

        if(!str->buf){
            perror("string_realloc: failed to reallocate buffer");
            exit(3);
        }
    }
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
	if (str)
	{
		// Puffer und Struct freigeben
		free(str->buf);
		free(str);
	}
}

void string_concat(string *str, char *src) {
    int len = strlen(src);

    if(!str || !src){
        perror("str or src is null");
        exit(2);
    }

    if(str->len + len > str->capacity)
        string_realloc(str, (str->len + len) * 2);

    memcpy((str->buf+str->len), src, len);
    str->len += len;
}

void string_add_char(string* str, char c)
{
    if (str->capacity < str->len + 1)
        string_realloc(str, (str->len + 1) * 2);

    str->buf[str->len++] = c;
}

void string_strip(string* str)
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
}

bool string_compare(string* string1, string* string2){

    if(string1->len != string2->len) //sanity check
        return false;

    return memcmp(string1->buf, string2->buf, string1->len) == 0;
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

    if((dst->len + src->len) > dst->capacity)
        string_realloc(dst, (dst->len + src->len)*2);

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

    *splits = 1;

    //count splitters
    for (int i = 0; i < str->len; i++) {
        if(str->buf[i] == splitter)
            (*splits)++;
    }

    //i am actually sorry for this, but its the easiest way
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
	    string_realloc(str, (str->len + 1) * 2);

	str->buf[str->len] = '\0';
	return str;
}

long string_to_long(string *str, int base) {
    string_terminate(str); //to be sure
    return strtol(str->buf, NULL, base);
}

bool string_compare_cstr(string *string1, char *string2) {
    string* str = string_new(strlen(string2));
    string_concat(str, string2);

    bool b = string_compare(string1, str);

    string_free(str);

    return b;
}

string *string_new_from_cstr(char *str) {
    string* b = string_new(strlen(str)*2);
    string_concat(b, str);

    return b;
}

string *string_join(string **splitted, int splits, char separator) {
    if(!splitted || splits == 0)
        return NULL;

    if(splits == 1)
        return string_copy(*splitted);

    string* str = string_new(1);

    string_concat_str(str, splitted[0]);

    for (int i = 1; i < splits; ++i) {
        if(splitted[i]) {

            string_add_char(str, separator);
            string_concat_str(str, splitted[i]);

        } else { // should never happen
            string_free(str);
            return NULL;
        }
    }

    return str;
}

void string_insert(string *dst, string *src, int index) {
    if((dst->len + src->len) > dst->capacity)
        string_realloc(dst, (dst->len + src->len)*2);

    memmove(dst->buf+index+src->len, dst->buf+index, dst->len-index);
    memcpy(dst->buf+index, src->buf, src->len);
    dst->len += src->len;
}

void string_insert_cstr(string *dst, char *src, int index) {
    string* srcStr = string_new_from_cstr(src);
    string_insert(dst, srcStr, index);
    string_free(srcStr);
}

bool string_startswith(string *str, string *needle) {

    if(needle->len >= str->len)
        return false;

    int strLen = str->len;
    str->len = needle->len;

    bool b = string_compare(str, needle);

    str->len = strLen;

    return b;
}

bool string_startswith_cstr(string *str, char *needle) {
    string* strNeedle = string_new_from_cstr(needle);

    bool b = string_startswith(str, strNeedle);

    string_free(strNeedle);

    return b;
}
