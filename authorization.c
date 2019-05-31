//
// Created by gi-user on 28.05.19.
//

#include "authorization.h"
#include "hash.h"
#include "base64.h"
#include <unistd.h>
#include <openssl/sha.h>



#define PATH_CAPACITY 2083
#define PATH_CAPACITY_ABSOLUTE (PATH_CAPACITY + 200)


bool abfrage_authorizaition (HashList * hashlist ){
    Hash * hash= SHL_find_key_cstr(hashlist,"authorization");
    if (hash){
        return true;
    }
    else
        return false;
}

string * password_to_sha1Base64(HashList* hashlist) {
    Hash *passwort = SHL_find_key_cstr(hashlist, "authorization");
    string *pw = string_copy(passwort->value);
    int splits;
    string **split = string_split_cstr(pw, " ", &splits);
    string_free(pw);
    pw = split[1];
    char *encode = base64_decode(pw->buf, pw->len, &pw->len);
    string_free_stringlist(split, splits);
    string *st_encode = string_new_from_cstr(encode);
    string **name_pw = string_split_cstr(st_encode, ":", &splits);
    unsigned char *hash[SHA_DIGEST_LENGTH];
    SHA1(name_pw[1]->buf,name_pw[1]->len,hash);
    size_t len;
    char *final = base64_encode(hash, SHA_DIGEST_LENGTH,&len);
    string *str = string_new_from_cstr(final);
    return str;
}


bool passwort_abfrage_authorizaition(HashList* hashlist) {


    Hash * passwort = SHL_find_key_cstr(hashlist,"authorization");
    string * pw = string_copy(passwort->value);
    int splits = 2;
    string ** split = string_split_cstr(pw," ",&splits);
    string_free(pw);
    pw = split[1];
    char *encode = base64_decode(pw->buf, pw->len, &pw->len);
    string_free_stringlist(split, splits);
    string *st_encode = string_new_from_cstr(encode);
    string **name_pw = string_split_cstr(st_encode, ":", &splits);
    char *pw2 = string_terminate(name_pw[1])->buf;
    unsigned char *hash[SHA_DIGEST_LENGTH];
    SHA1(pw2,name_pw[1]->len,hash);
    if (1) {
        char *pwencode = base64_decode(pw->buf, pw->len, &pw->len);
        string_free_stringlist(split, 2);
        char *p = calloc(sizeof(char), 50);
        int temp = false; //read_pw_list(hashlist);

        if (temp == true) {
            return true;
        }
        return false;
    }
}



bool authorizaition (HashList * hashlist){
    int temp = abfrage_authorizaition(hashlist);
    if (temp == true){
        Hash * passwort = SHL_find_key_cstr(hashlist,"authorization");
        temp  = /*false;*/read_pw_list (passwort);

       if (temp == true ) {
            return true;
        }else {
           return false;
       }
    }
    return false;
}



string * pw_rood(){
    string* ht_passwd_Dir = string_new(PATH_CAPACITY_ABSOLUTE);
    getcwd(ht_passwd_Dir->buf, PATH_CAPACITY_ABSOLUTE);
    ht_passwd_Dir->len = strlen(ht_passwd_Dir->buf);

    string_concat(ht_passwd_Dir, "/../htpasswd/htpasswd");
    string_terminate(ht_passwd_Dir);

    string* absolute_Ht_passwd_Dir = string_new(PATH_CAPACITY_ABSOLUTE);
    realpath(ht_passwd_Dir->buf, absolute_Ht_passwd_Dir->buf);
    absolute_Ht_passwd_Dir->len = strlen(absolute_Ht_passwd_Dir->buf);

    string_free(ht_passwd_Dir);

    return absolute_Ht_passwd_Dir;
}

bool read_pw_list(Hash *hash){
     string * pw_list_pfad_st = pw_rood();
    //string *pw_list_pfad_st = string_new_from_cstr("/home/bob/Dokumente/pwlist");
    string * pw_list_pfad = string_terminate(pw_list_pfad_st);
    string_free(pw_list_pfad_st);

    FILE *pw = fopen(pw_list_pfad->buf,"rb");
//    string_free(pw_list_pfad);
//TODO  pw_list_pfad freen (funktioniert nicht warum auch immer!?)

    int exit = 0;
    bool rueck = false;

    if (pw == NULL) {
        printf("Datei konnte nicht geoeffnet werden.\n");
    } else {
        string *name_str = string_new(255);
        string *pw_str = string_new(255);
        char pw_tmp;
        char tmp;

        while((tmp = fgetc(pw))!=EOF && exit != 1){
            switch(tmp){
                case '\n': // New Line
                    //free(name_str->buf);
                    break;
                case ':': // :
                    if (string_compare(hash->key,name_str)){

                        while ((pw_tmp = fgetc(pw)) != '\n'){
                            if (pw_tmp == EOF){
                                exit = 1;
                                break;
                            }
                            string_concat(pw_str, &pw_tmp);
                        }
                        // das eingegebene PW muss codiert werden
                        if (string_compare(pw_str,hash->value)){
                            //TODO BOB: FREE zeug hinzufuegen
                            return rueck = true;
                        } else {
                            //TODO AMADEUS: FREE zeug hinzufuegen
                            return rueck = false;
                        }
                    } else{

                    }
                    break;

                default:
                    string_concat(name_str,&tmp);
                    break;
            }

        }
    }
    //TODO Bartek: FREE zeug hinzufuegen
    fclose(pw);
    return rueck = false;
}