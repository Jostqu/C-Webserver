//
// Created by gi-user on 28.05.19.
//

#include "authorization.h"
#include "hash.h"
#include "base64.h"
#include <unistd.h>
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

    Hash * passwort = SHL_find_key_cstr(hashlist,"authorization");
    string * pw = string_copy(passwort->value);
    int splits = 2;
    string ** split = string_split_cstr(pw," ",&splits);
    string_free(pw);
    pw = split[1];
    char * pwencode= base64_decode(pw->buf,pw->len,&pw->len);
    string_free_stringlist(split,2);
    char * p = calloc(sizeof(char),50);
    if ( 1){
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