///
/// \author Björn Marx
/// \date 26/04/19
/// \file response.h
///

#ifndef PSE_RESPONSE_H
#define PSE_RESPONSE_H

#include <unistd.h>
#include <sys/socket.h>
#include "hash.h"
#include "helpers.h"

/*!
 * \brief struct defining supported HTTP response codes
 * \author Björn Marx
 */
typedef enum {

    OK = 200,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_IMPLEMENTED = 501

}HttpResponseCodes;

/*!
 * \brief converts the responsecode to a string
 *
 * \author Björn Marx
 *
 * @return code as string
 */
char* code_to_string(HttpResponseCodes code);

/*!
 * \brief simple wrapper to build HTTP response
 *
 * Please ensure your Hashes are used the following way:
 *              key   -> field name
 *              value -> field value
 * the function will add the ':' separator after the name itself.
 *
 * the fuction will return NULL on failure
 *
 * \author Björn Marx
 *
 * @param code desired HTTP response code, chosen from HttpResponseCodes enum
 * @param fields list of header fields
 * @return Http response as string
 */
string* build_http_response_header(HttpResponseCodes code, HashList *fields);

/**
 * \brief Nutzt die build_http_response-Funktion um eine Antwort an den Client zusenden
 * \author Marcel Weski
 * @param targetStream socket-Instanz oder STDOUT-Stream
 * @param code responseCode von der parse_http_request-Funktion
 * @param path vom Client angeforderter Pfad
 * @param staticPage ggf. eine statische Seite. z.B. /debug
 */
void send_http_response(int targetStream, HttpResponseCodes code, string* path, string* staticPage);

#endif //PSE_RESPONSE_H
