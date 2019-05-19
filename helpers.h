///
/// \file helpers.h
/// \author Björn Marx
///

#ifndef PSE_HELPERS_H
#define PSE_HELPERS_H

#include <sys/stat.h>
#include "string.h"

/*!
 * \brief enum containing supported version(s)
 * \author Björn Marx
 */
typedef enum {
    UNSUPPORTED,
    VERSION_1_1
}HTTPVersion;


/*!
 * \brief enum containing supported methods
 * \author Björn Marx
 */
typedef enum {
    INVALID,
    GET,
    POST,
    HEAD,
    PUT,
    PATCH,
    DELETE,
    TRACE,
    OPTIONS,
    CONNECT
}HTTPMethod;

/*!
 * \brief checks whether the HTTP version is supported or not
 * note that the string must be valid by HTTP header standard ("HTTP/1.1", others not supported)
 *
 * \author Björn Marx
 *
 * @param ver a string containing the HTTP version
 * @return Version or unsupported
 */
HTTPVersion validate_version(string* ver);

/*!
 * \brief gets the HTTP method from a string
 *
 * \author Björn Marx
 *
 * @param method string containing the method
 * @return Method or Invalid
 */
HTTPMethod get_method_from_string(string* method);

/*!
 * \brief gets content type of file with help of the file command
 *
 * \warning as it seems to be forgotten otherwise: YOU HAVE TO FREE THE RETURNED STRING YOURSELF!
 *
 * \author Björn Marx
 *
 * @param path path to file
 * @return content type and charset as string, separated by ';'
 */
string* get_content_type(char* path);

/*!
 * \brief decodes url, obviously
 *
 * replaces the %xx placeholder from HTML with their corresponding ascii chars
 *
 * \author Björn Marx
 *
 * @param str a string with html's weird %XX placeholders
 */
void url_decode(string *str);

/**
 * \brief checks if a given path is a file
 * \author Marcel Weski
 * @param path path of file or directory
 * @return true if path is a existing file
 */
bool isfile(string* path);

/**
 * \brief checks if a file exists
 * \author Marcel Weski
 * @param path filepath
 * @return true if file exists
 */
bool file_exists(string* path);

/**
 * \brief returns the file size of a opened file (with fopen)
 * \author Marcel Weski
 * @param fp pointer of opened file
 * @return file size
 */
long get_file_size(FILE* fp);

#endif //PSE_HELPERS_H
