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


void read_pw_list(Hash *hash){
    FILE *pw = fopen("","r");
    int tmp;
    int exit = 0;
    if (pw == NULL) {
        printf("Datei konnte nicht geoeffnet werden.\n");
    } else {
        string *name_str = string_new(255);

        while((tmp = fgetc(pw))!=EOF && exit == 0){
            string *tmp_str = int_to_string(tmp);

            switch(tmp){
                case 10: // New Line
                    break;

                case 58: // :
                    if (string_compare(hash->key,name_str)){
                        int pw_tmp;
                        string *pw_str = string_new(255);

                        while ((pw_tmp = fgetc(pw)) != EOF && pw_tmp != 10){

                        }
                    } else{

                    }
                    break;

                default:
                    string_concat_str(name_str,tmp_str);
                    break;
            }

            string_free(tmp_str);
        }
    }
    fclose(pw);
}