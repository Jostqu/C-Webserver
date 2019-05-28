//
// Created by gi-user on 28.05.19.
//

#ifndef PSE20_AUTHORIZATION_H

#include "hash.h"

/**
 * Checks whether an authorization header exists
 * @author Luise
 * @param hashlist Anfrage
 * @return bool true / false
 */
bool abfrage_authorizaition (HashList * hashlist );

/**
 * Checks if username and password are correct.
 * @authors Jost, Mark & Luise
 * @param hashlist Anfrage
 * @return bool true / false
 */
bool passwort_abfrage_authorizaition(HashList* hashlist) ;

/**
 *  Checks whether an authorization header exists and if
 *  username and password are correct.
 * @author Luise
 * @param hashlist
 * @return true / false
 */
bool authorizaition (HashList * hashlist);

void read_pw_list(Hash *hash);

#define PSE20_AUTHORIZATION_H

#endif //PSE20_AUTHORIZATION_H
