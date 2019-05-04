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
 * \brief Struct, das die Kapazität (max. Stringlänge), Länge des Strings und den Pointer auf die eigentlichen Zeichen enthält
 * \author Marcel Weski
 * @var capacity max. Stringlänge
 * @var len tatsächliche Stringlänge
 * @var buf Pointer auf die eigentlichen Zeichen
 */
typedef struct string_struct
{
    size_t capacity;
    size_t len;
    char *buf;
} string;

/**
 * \brief Erstellt einen neuen String mit der gegebenen Kapazität (max. Stringlänge)
 * Der String wird mittels malloc erzeugt und muss daher mit string_free selber wieder freigegeben werden!
 * \author Marcel Weski
 * @param capacity maximale Anzahl an Zeichen, die vor realloc reingeschrieben werden können
 * @return Pointer auf das neue String-Struct
 */
string* string_new(size_t capacity);

/*!
 * \brief creates new string from given char*
 *
 * \author Björn Marx
 *
 * @param str a string
 * @return new string with content of str
 */
string* string_new_from_cstr(char* str);

/**
 * \brief Erstellt eine identische Kopie des übergebenen Strings
 * Der String wird mittels malloc erzeugt und muss daher mit string_free selber wieder freigegeben werden!
 * \author Marcel Weski
 * @param str der zu kopierende String-Pointer
 * @return Der kopierte String als Pointer
 */
string* string_copy(string* str);

/**
 * \brief Gibt den Speicherbereich eines Strings wieder frei
 * \author Marcel Weski
 * @param str Der freizugebene String
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
void string_concat(string* str, char* src); //len removed by request of marsel

/*!
 * \brief concat string typed string src to dst
 * equivalent to string_concat
 *
 * \author Björn Marx
 *
 * @param dst destination
 * @param src source
 */
void string_concat_str(string* dst, string*src);

/**
 * \brief Fügt ein Zeichen an das Ende des Strings ein und vergrößert die Kapazität gegenfalls
 * \author Marcel Weski
 * @param str String, an den das Zeichen angehängt werden soll
 * @param c Das anzuhängende Zeichen
 */
void string_add_char(string* str, char c);

/**
 * \brief Entfernt leere Zeichen am Anfang und am Ende des Strings
 * Alle Leerzeichen, Absätze und Backspaces werden am Anfang und am Ende des String entfernt.
 * \author Marcel Weski
 * @param str String
 * @return Selber String wie Argument (ändert sich durch Aufruf dieser Funktion nie!)
 */
string* string_strip(string* str);

/**
 * \brief vergleicht 2 strings
 * vergleicht 2
 *
 * \author Björn Marx
 *
 * @param string1
 * @param string2
 * @return true if string1 == string2
 */
bool string_compare(string* string1, string* string2);

/**
 * \brief Gibt einen String mit Absatz auf der Konsole aus
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
 * its a basic method with a rather bad runtime of O(n²)
 *
 * \warning function returns NULL when any routine fails, this *might* (but should not happen) cause a memory leak when some memory allocation fails
 *
 * \author Björn Marx
 *
 * @param splits pointer to integer to store stringlists length in
 * @param str an string
 * @param splitter charakter defining when to split
 * @return list of strings
 */
string** string_split(string* str, char splitter, int* splits);

/**
 * \brief Terminiert einen String mit \0, um diesen z.B. bei fopen übergeben zu können
 * \author Marcel Weski
 * @param str zu terminierender String
 * @return Selber String wie Argument (ändert sich durch Aufruf dieser Funktion nie!)
 */
string* string_terminate(string* str);

/*!
 * \brief little extension to the string_compare
 *
 * @param string1 a string
 * @param string2 a cstring (char*)
 * @return true if equal
 */
bool string_compare_cstr(string *string1, char *string2);
#endif //PSE_STRING_H
