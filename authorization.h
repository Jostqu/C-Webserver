
///
/// \author PSE 17
/// \date 28.05.2019
/// \brief
/// \file authorization.h
///

#ifndef PSE20_AUTHORIZATION_H

#include "hash.h"
#include <openssl/sha.h>

/**
 * Checks whether an authorization header exists
 * @author Luise
 * @param hashlist Anfrage
 * @return bool : true / false
 */
bool abfrage_authorizaition (HashList * hashlist );


/**
 *  Reads the password and the name from the hashlist and returns a Hash with the Name and the sha1 and Base 64
 *  encoded Password.
 * @author Jost
 * @warning The returned Hash must be freed
 * @param hashlist  HaschList with the Name and Password.
 * @return hash that contains the name(clear text) as the key, and the password (SHA1 and Base64 encoded) as the value
 */
Hash password_to_sha1Base64(HashList* hashlist);
/**
 * Increment the File-Pointer char by char and checks them for EOF or the delimiter.
 * Writes the chars at the end of the String str
 * @authors Marc
 * @param file the file that get checked
 * @param str the edited String
 * @param delimiter the searched char
 * @return return True if EOF is reached, else it is false
 */
bool file_char_into_str(FILE *file,string *str,char delimiter);

/**
 * Increment the File-Pointer char by char and checks them for EOF or the delimiter.
 * @authors Marc
 * @param file the file that get checked
 * @param delimiter the searched char
 * @return return True if EOF is reached, else it is false
 */
bool file_skip(FILE *file,char delimiter);
/**
 * Checks if username and password are a correct pair.
 * @authors Jost, Marc
 * @param hash that contains the name(clear text) as the key, and the password (SHA1 encoded) as the value
 * @return bool is true if the password and name match the htpasswd file
 *
 */
bool read_pw_list(Hash *hash);

/**
 * Created the Password-rood-string
 * @author Luise
 * @warning the return string must been freed
 * @return string : password_rood
 */
string * pw_rood();

/**
 * Checks whether an authorization header exists and if
 * username and password are correct.
 * @author Luise
 * @param hashlist
 * @return bool : true / false
 */
bool authorizaition (HashList * hashlist);



#define PSE20_AUTHORIZATION_H

#endif //PSE20_AUTHORIZATION_H
