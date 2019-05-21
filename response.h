///
/// \author Björn Marx
/// \author Marcel Weski
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

}HttpResponseCode;

/*!
 * \brief converts the responsecode to a string
 *
 * \author Björn Marx
 *
 * @return code as string
 */
char* code_to_string(HttpResponseCode code);

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
string* build_http_response_header(HttpResponseCode code, HashList *fields);

/**
 * \brief uses the build_http_response_header function to send a response to the client
 * \author Marcel Weski
 * @param targetStream socket or stdout stream
 * @param code responseCode returned by the parse_http_request function
 * @param path requested absolute file path
 * @param staticPage contains a string if /debug was requested
 */
void send_http_response(int targetStream, HttpResponseCode code, string* path, string* staticPage);

#endif //PSE_RESPONSE_H
