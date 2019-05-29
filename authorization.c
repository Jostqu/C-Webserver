//
// Created by gi-user on 28.05.19.
//

#include "authorization.h"
#include "hash.h"
#include "base64.h"
#include <unistd.h>


bool abfrage_authorizaition (HashList * hashlist ){
    Hash * hash= SHL_find_key_cstr(hashlist,"authorization");
    if (hash){
        return true;
    }
    else
        return false;
}

bool passwort_abfrage_authorizaition(HashList* hashlist) {

   /*
    Hash * passwort = SHL_find_key_cstr(hashlist,"authorization");
    string * pw = string_copy(passwort->value);
    int splits = 2;
    string ** split = string_split_cstr(pw," ",&splits);
    string_free(pw);
    pw = split[1];
    char * pwencode= base64_decode(pw->buf,pw->len,&pw->len);
    string_free_stringlist(split,2);
    char * p = calloc(sizeof(char),50);
*/
    int temp = false; //read_pw_list(hashlist);

    if ( temp == true){
        return true;
    }
     return false;


}



bool authorizaition (HashList * hashlist){
    int temp = abfrage_authorizaition(hashlist);
    if (temp == true){
        temp  = passwort_abfrage_authorizaition (hashlist);
       if (temp == true ) {
            return true;
        }else {
           return false;
       }
    }
    return false;
}

#define PATH_CAPACITY 2083
#define PATH_CAPACITY_ABSOLUTE (PATH_CAPACITY + 200)


#define PATH_CAPACITY 2083
#define PATH_CAPACITY_ABSOLUTE (PATH_CAPACITY + 200)

string * pw_pfad(){
    string* ht_passwd_Dir = string_new(PATH_CAPACITY_ABSOLUTE);
    getcwd(ht_passwd_Dir->buf, PATH_CAPACITY_ABSOLUTE);
    ht_passwd_Dir->len = strlen(ht_passwd_Dir->buf);

    string_concat(ht_passwd_Dir, "/htpasswd");
    string_terminate(ht_passwd_Dir);

    string* absolute_Ht_passwd_Dir = string_new(PATH_CAPACITY_ABSOLUTE);
    realpath(ht_passwd_Dir->buf, absolute_Ht_passwd_Dir->buf);
    absolute_Ht_passwd_Dir->len = strlen(absolute_Ht_passwd_Dir->buf);

    string_free(ht_passwd_Dir);

    return absolute_Ht_passwd_Dir;
}

bool read_pw_list(Hash *hash){

    //string * pw_list_pfad_st = pw_pfad();
    string *pw_list_pfad_st = string_new_from_cstr("/home/bob/Dokumente/pwlist");
    string * pw_list_pfad = string_terminate(pw_list_pfad_st);
    //string_free(pw_list_pfad_st);

    FILE *pw_list = fopen(pw_list_pfad->buf,"rb");
    string_free(pw_list_pfad);

    bool exit = false;

    if (pw_list == NULL) {
        printf("Datei konnte nicht geoeffnet werden.\n");
    } else {
        string *name_str = string_new(255);
        string *pw_str = string_new(255);
        char tmp;

        while((tmp = fgetc(pw_list))!=EOF && exit != true){
            switch(tmp){
                case '\n': // New Line
                    printf("Error in der PW-Liste\n");
                    exit = true;
                    break;
                case ':': // :
                    if (string_compare(hash->key,name_str)){

                        while ((tmp = fgetc(pw_list)) != '\n' && exit != true){
                            if (tmp == EOF){
                                exit = true;
                            } else {
                                string_concat(pw_str, &tmp);
                            }
                        }
                        // das eingegebene PW muss codiert werden

                        if (string_compare(pw_str,hash->value)){

                            string_free(name_str);
                            string_free(pw_str);
                            return true;
                        } else {

                            string_free(name_str);
                            string_free(pw_str);
                            return false;
                        }
                    } else {
                        free(name_str->buf);
                        name_str->len = 0;
                        while ((tmp = fgetc(pw_list)) != '\n' && exit != true) {
                            if (tmp == EOF) {
                                exit = true;
                            }
                        }
                    }
                    break;

                default:
                    string_concat(name_str,&tmp);
                    break;
            }
        }
        //TODO Bartek: FREE zeug hinzufuegen
        fclose(pw_list);
        return false;
    }
    return false;
}