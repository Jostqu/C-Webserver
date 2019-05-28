//
// Created by gi-user on 28.05.19.
//

#ifndef PSE20_AUTHORIZATION_H

#include "hash.h"


int abfrage_authorizaition (HashList * hashlist );

int passwort_abfrage_authorizaition(HashList* hashlist) ;

int authorizaition (HashList * hashlist);

void read_pw_list(Hash *hash);

#define PSE20_AUTHORIZATION_H

#endif //PSE20_AUTHORIZATION_H
