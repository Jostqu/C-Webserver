//
// Created by Marcel on 26.04.2019.
//

#ifndef PSE_HTTP_REQUEST_H
#define PSE_HTTP_REQUEST_H

#include <ctype.h>
#include "hash.h"
#include "response.h"
#include "helpers.h"

/**
 * \brief Struct, das Informationen aus der HTTP-Request enthält
 * \author Marcel Weski
 * @var fields Alle Felder als HashList (key, value Tupel)
 * @var data Void-Pointer auf das erste Zeichen der Daten
 */
typedef struct http_request_struct
{
    HTTPMethod method;
    string* path;
    HTTPVersion version;

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
    PARSING_RESOURCE,
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
void free_http_request(HttpRequest* httpRequest);

/**
 * \brief Validates the requested resource
 * Stellt sicher, dass der Client sich nur innerhalb des DocumentRoots bewegt und gibt entsprechende ResponseCodes zurück.
 * @param path Pfad vom Client
 * @param validatedPath der neue absolute Pfad, wenn die Datei existiert. Muss selber freigegeben werden!
 * @return OK wenn die Datei existiert und der Client Zugriff haben darf. 403 bei Zugriffen außerhalb des DocumentRoots. 404 wenn die Datei nicht vorhanden ist.
 */
HttpResponseCodes validate_resource(string *resource, string **path);

/**
 * \brief Parsed den eingehenden Void-Buffer und gibt ein Struct mit den Informationen im Header zurück
 * \author Marcel Weski
 * @param buffer Void-Pointer auf den Request-Puffer
 * @param bufferSize Größe des Puffers
 * @param httpRequest der gefüllte HttpRequest-Struct
 * @param staticPage ggf. eine statische Seite. z.B. bei /debug
 * @return int der Response-Code
 */
HttpResponseCodes parse_http_request(char* buffer, size_t bufferSize, HttpRequest* httpRequest, string** staticPage);

#endif //PSE_HTTP_REQUEST_H
