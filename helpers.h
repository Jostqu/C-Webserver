///
/// \file helpers.h
/// \author Björn Marx
///

#ifndef PSE_HELPERS_H
#define PSE_HELPERS_H

#include "string.h"

/*!
 * \brief gets content type of file with help of the file command
 *
 * \author Björn Marx
 *
 * @param path path to file
 * @return content type and charset as string, separated by ';'
 */
string* get_content_type(char* path);

/*!
 * \brief decodes url
 *
 * replaces the %xx placeholder from HTML with their corresponding ascii chars
 *
 * \author Björn Marx
 *
 * @param str a string with html's weird %XX placeholders
 */
void url_decode(string *str);

#endif //PSE_HELPERS_H
