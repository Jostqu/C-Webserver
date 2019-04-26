///
/// \author Björn Marx
/// \date 26/04/19
/// \file response.h
///

#ifndef PSE_RESPONSE_H
#define PSE_RESPONSE_H

#include "hash.h"

/*!
 * \brief struct defining supported HTTP response codes
 */
typedef enum {

    OK = 200,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405

}HttpResponseCodes;

/*!
 * \brief converts the responsecode to a string
 *
 * \author Björn Marx
 *
 * @return code as string
 */
string* code_to_string(HttpResponseCodes code);

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
string* build_http_response(HttpResponseCodes code, HashList* fields, string* body);

#endif //PSE_RESPONSE_H
