//
// Created by Marcel on 26.04.2019.
//

#ifndef PSE_HTTP_REQUEST_H
#define PSE_HTTP_REQUEST_H

#include <ctype.h>
#include "hash.h"

/**
 * \brief Struct, das Informationen aus der HTTP-Request enthält
 * \author Marcel Weski
 * @var fields Alle Felder als HashList (key, value Tupel)
 * @var data Void-Pointer auf das erste Zeichen der Daten
 */
typedef struct http_request_struct
{
    // TODO: Methode
    // TODO: Pfad
    // TODO: Version

    HashList* fields;
    void* data;
} HttpRequest;

/**
 * \brief Enum, für den aktuellen Parsing-State
 * Da zeichenweise durch den Puffer gegangen wird, wird hiermit festgehalten was gerade geparst wird
 * \author Marcel Weski
 */
typedef enum http_request_parsing_state
{
    PARSING_METHOD,
    PARSING_PATH,
    PARSING_VERSION,
    PARSING_FIELD_KEY,
    PARSING_FIELD_VALUE,
    PARSING_DATA
} HttpRequestParsingState;

/**
 * \brief Gibt die HashList im HttpRequest-Struct wieder frei
 * \author Marcel Weski
 * @param httpRequest das freizugebene HttpRequest-Struct
 */
void free_http_request(HttpRequest httpRequest);

/**
 * \brief Parsed den eingehenden Void-Buffer und gibt ein Struct mit den Informationen im Header zurück
 * \author Marcel Weski
 * @param buffer Void-Pointer auf den Request-Puffer
 * @param bufferSize Größe des Puffers
 * @param httpRequest der gefüllte HttpRequest-Struct
 * @return int der Response-Code
 */
int parse_http_request(void* buffer, size_t bufferSize, HttpRequest* httpRequest);

#endif //PSE_HTTP_REQUEST_H
