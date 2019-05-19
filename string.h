///
/// \file string.h
/// \author Björn Marx
/// \author Marcel Weski
///

#ifndef PSE_STRING_H
#define PSE_STRING_H

#include <malloc.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

/**
 * \brief contains the capacity (max. length and buffer size), length of the string, pointer to buffer that contains the chars
 * \author Marcel Weski
 * @var capacity max. length (buffer size)
 * @var len actual string length
 * @var buf pointer to chars
 */
typedef struct string_struct
{
    size_t capacity;
    size_t len;
    char *buf;
} string;

/**
 * \brief creates a new string with the given capacity
 * allocates memory with calloc that needs to be freed by string_free
 * \author Marcel Weski
 * @param capacity max. amount of chars that can be saved before reallocating
 * @return pointer to string struct
 */
string* string_new(size_t capacity);

/*!
 * \brief creates new string from given char*
 *
 * \author Björn Marx
 *
 * @param str a string
 * @return new string with content of str, NULL if @param str is NULL
 */
string* string_new_from_cstr(char* str);

/*!
 * \brief creates new string from given char* but limits at length
 *
 * \author Björn Marx
 *
 * @param str a char*
 * @param len length of char*
 * @return new string
 */
string* string_new_from_carray(char* str, int len);

/**
 * \brief creates a identical copy of a given string
 * allocates memory with calloc that needs to be freed by string_free
 * \author Marcel Weski
 * @param str string to copy
 * @return pointer to the new string struct
 */
string* string_copy(string* str);

/**
 * \brief frees a string
 * \author Marcel Weski
 * @param str string to free
 */
void string_free(string* str);

/**
 * \brief appends src to str
 *
 * \author Björn Marx
 *
 * @param str string to append to
 * @param src string to append
 */
void string_concat(string* str, char* src); //len removed by request of marcel

/*!
 * \brief concat string typed string src to dst
 * equivalent to string_concat
 *
 * \author Björn Marx
 *
 * @param dst destination
 * @param src source
 */
void string_concat_str(string* dst, string* src);

/**
 * \brief adds a char to a string that may be reallocated if capacity isn't big enough
 * \author Marcel Weski
 * @param str string to add char to
 * @param c the char
 */
void string_add_char(string* str, char c);

/**
 * \brief removes all spaces at begin and end of a string
 * \author Marcel Weski
 * @param str string to be stripped
 */
void string_strip(string* str);

/**
 * \brief compares two strings
 * compares two string with memcmp
 *
 * \author Björn Marx
 *
 * @param string1
 * @param string2
 * @return true if string1 == string2
 */
bool string_compare(string* string1, string* string2);

/**
 * \brief prints a string onto the console
 * \author Marcel Weski
 * @param str String
 */
void string_print(string* str);

/*!
 * \brief int to string function
 *
 * \author Björn Marx
 *
 * @param i an integer
 * @return string containing i
 */
string* int_to_string(int i);

/*!
 * \brief wrapper for strtol
 *
 * \author Björn Marx
 *
 * @param str
 * @return
 */
long string_to_long(string *str, int base);

/*!
 * \brief splits a string by c
 * its a basic method with a rather bad runtime of O(2n)
 * if there is a splitter, the list will contain an empty string at the end
 *
 * \author Björn Marx
 *
 * @param splits pointer to integer to store stringlists length in
 * @param str an string
 * @param splitter charakter defining when to split
 * @return list of strings
 */
string** string_split(string* str, char splitter, int* splits);

/*!
 * \brief splits a string, but uses a string as splitter argument
 * uses the same "algorithm" as string_split but adapted to use string as splitter
 *
 * \author Björn Marx
 *
 * @param str string to split
 * @param splitter string that splits
 * @param splits number of splits
 * @return array of splitted string
 */
string** string_split_string(string* str, string* splitter, int* splits);

/*!
 * \brief \see string_split_string
 *
 * \author Björn Marx
 */
string** string_split_cstr(string* str, char* splitter, int* splits);

/*!
 * \brief recombines list from string_split
 * function returns null on failure or if splitted is null or if splits is 0
 * \author Björn Marx
 * @param splitted list of splitted strings
 * @param splits number of strings in splitted
 * @param separator char to put between individual strings
 * @return string containing all substrings of splitted
 */
string* string_join(string** splitted, int splits, char separator);

/**
 * \brief terminates a string (adds a \0 to the end). Needed for functions like fopen
 * \author Marcel Weski
 * @param str string to terminate
 * @return same string as argument (never changes, just to make code more compact)
 */
string* string_terminate(string* str);

/*!
 * \brief little extension to the string_compare
 * \author Björn Marx
 * @param string1 a string
 * @param string2 a cstring (char*)
 * @return true if equal
 */
bool string_compare_cstr(string *string1, char *string2);

/*!
 * \brief inserts src at index
 *
 * \author Björn marx
 *
 * @param dst string to insert to
 * @param src string to insert from
 * @param index explains itself
 */
void string_insert(string* dst, string* src, int index);

/*!
 * \brief extension of string_insert to support char*
 * \author Björn Marx
 * \see string_insert
 */
void string_insert_cstr(string* dst, char* src, int index);

/*!
 * \brief checks is str starts with needle
 * uses string_compare \see string_compare for detailed info
 *
 * \author Björn Marx
 *
 * @param str string to check if it starts with needle
 * @param needle a string
 * @return true if string starts with needle
 */
bool string_startswith(string* str, string* needle);

/*!
 * \brief checks if str starts with needle
 * \author Björn Marx
 * \see string_startswith to see how it works
 */
bool string_startswith_cstr(string* str, char* needle);

/*!
 * \brief compares end of string with another string
 * \author Marcel Weski
 * @param str string to compare end with
 * @param part end part of str
 * @return true, if part is equal to last chars of str
 */
bool string_endswith(string* str, string* part);

/*!
 * \brief compars end of string with another string (char pointer)
 * \see string_endswith
 */
bool string_endswith_cstr(string* str, char* part);
#endif //PSE_STRING_H
