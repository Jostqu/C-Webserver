//
// Created by Luise on 28.05.19.
//
/// \author PSE 17
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
 * verschlüsselung das Passworts
 * @param hashlist
 * @return
 */
bool passwort_abfrage_authorizaition(HashList* hashlist);

Hash password_to_sha1_Base64(HashList* hashlist);


/**
 * Checks if username and password are correct.
 * @authors Jost, Mark

 */
bool read_pw_list(Hash *hash);

/**
 * Created the Password-rood-string
 * @author Luise
 * @return string : password_rood
 */
string * pw_rood();

bool check_char_by_sh1_base64_char(string *pwcheck,string *pwtrue);
/**
 *  Checks whether an authorization header exists and if
 *  username and password are correct.
 * @author Luise
 * @param hashlist
 * @return bool : true / false
 */
bool authorizaition (HashList * hashlist);



#define PSE20_AUTHORIZATION_H

#endif //PSE20_AUTHORIZATION_H
