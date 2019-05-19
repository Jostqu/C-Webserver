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
 * \brief contains all information from request
 * \author Marcel Weski
 * @var method requested method
 * @var resource requested resource
 * @var version requested http version
 * @var fields HashList of requested fields (key: value)
 * @var data string of received data
 */
typedef struct http_request_struct
{
    HTTPMethod method;
    string* resource;
    HTTPVersion version;
    HashList* fields;
    string* data; // for future use
} HttpRequest;

/**
 * \brief frees the HttpRequest-Struct
 * \author Marcel Weski
 * @param httpRequest struct to free
 */
void free_http_request(HttpRequest* httpRequest);

/**
 * \brief validates the requested resource
 * Makes sure that the client is only able to access files within the DocumentRoot and returns the right response code
 * \author Marcel Weski
 * @param hashList fields of the request
 * @param resource requested resource
 * @param path the new absolute path from the resource, if the file exists. Must be freed!
 * @return 200, if the file exists. 403, if the access is not allowed (specific files and all files outside the document root). 404, if the file doesn't exists
 */
HttpResponseCode validate_resource(HashList* hashList, string *resource, string **path);

/**
 * \brief parses the received buffer, returns a response code and fills the httpRequest-Struct
 * \author Marcel Weski
 * @param request string of buffers
 * @param httpRequest the filled httpRequest. Must be freed!
 * @param staticPage returns the debug-Page if /debug is requested. Must be freed!
 * @return int the response code
 */
HttpResponseCode parse_http_request(string* request, HttpRequest** httpRequest, string** staticPage);

/**
 * \brief checks if the request string contains a valid header end ('\r\n\r\n')
 * @param strRequest the whole request as string
 * @param buffer the current buffer data
 * @param bufferSize the current buffer data size
 * @return true if end of header was found, otherwise false
 */
bool fill_request_string(string *strRequest, char *buffer, size_t bufferSize);

#endif //PSE_HTTP_REQUEST_H
