//
// Created by gi-user on 28.05.19.
//

#include "authorization.h"
int abfrage_authorizaition (HashList * hashlist ){
    Hash * hash= SHL_find_key_cstr(hashlist,"authorization");
    if (hash){
        return 1;
    }
    else
        return 0;
}

int passwort_abfrage_authorizaition(HashList* hashlist) {

    // DOTO Passwort überprüfen

    if (/*DOTO  Passwort ok*/ 1){
        return 1;
    }
    else{
        return 0;
    }

}

int authorizaition (HashList * hashlist){
    int temp = abfrage_authorizaition(hashlist);
    if (temp == 1){
        temp  = passwort_abfrage_authorizaition (hashlist);
        if (temp == 1 ) {
            return 1;
        }
    }
    return 0;
}