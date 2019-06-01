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

Hash password_to_sha1Base64(HashList* hashlist) {
    Hash *passwort = SHL_find_key_cstr(hashlist, "authorization");

    string *pw = string_copy(passwort->value);
    int splits1;
    int splits2;
    string **split = string_split_cstr(pw, " ", &splits1);
    string_free(pw);
    pw = split[1];
    unsigned char *encode = base64_decode(pw->buf, pw->len, &pw->len);
    string *st_encode = string_new_from_cstr(encode);
    string **name_pw = string_split_cstr(st_encode, ":", &splits2);
    unsigned char *hash[SHA_DIGEST_LENGTH];
    SHA1(name_pw[1]->buf,name_pw[1]->len,hash);
    size_t len;
    char *final = base64_encode(hash, SHA_DIGEST_LENGTH,&len);
    string *str = string_new_from_cstr(final);
    Hash returnHash = SH_create(string_copy(name_pw[0]),string_copy(str));

    string_free(str);
    string_free_stringlist(name_pw, splits2);
    string_free(st_encode);
    string_free_stringlist(split, splits1);
    //string_free(pw);
    free(final);
    free(encode);
    return returnHash;
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
        Hash  passwort = password_to_sha1Base64(hashlist);
        temp  = read_pw_list (&passwort);
            string_free(passwort.key);
            string_free(passwort.value);
//            free(&passwort);
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

bool read_pw_list(Hash* hash){
    string * pw_list_pfad_st = pw_rood();
    string * pw_list_pfad = string_terminate(pw_list_pfad_st);

    FILE *pw_list = fopen(pw_list_pfad->buf,"rb");
    string_free(pw_list_pfad);

    bool exit = false;
    if (pw_list == NULL) {
        printf("Datei konnte nicht geoeffnet werden.\n");
    } else {

        char tmp;

        string *name_str = string_new(255);
        string *pw_str = string_new(255);
        while(exit != true){

            if ((tmp = fgetc(pw_list))!= EOF) {
                switch (tmp) {
                    case '\n': // New Line
                        string_free(name_str);
                        break;
                    case ':': // :
                        if (string_compare(hash->key, name_str)) {

                            while ((tmp = fgetc(pw_list)) != '}' && exit != true) {
                                if (tmp == EOF) {
                                    string_free(name_str);
                                    string_free(pw_str);
                                    exit = true;
                                }
                            }
                            while ((tmp = fgetc(pw_list)) != '=' && exit != true) {
                                if (tmp == EOF) {
                                    string_free(name_str);
                                    string_free(pw_str);
                                    exit = true;
                                } else {
                                    string_concat(pw_str, &tmp);
                                }
                            }

                            if (string_compare(hash->value, pw_str)) {
                                fclose(pw_list);
                                string_free(name_str);
                                string_free(pw_str);
                                return true;
                            } else {
                                fclose(pw_list);
                                string_free(name_str);
                                string_free(pw_str);
                                return false;
                            }
                        } else {
                            free(name_str->buf);
                            name_str->buf = calloc(255,1);
                            name_str->len = 0;
                            while ((tmp = fgetc(pw_list)) != '\n' && exit != true) {
                                if (tmp == EOF) {
                                    string_free(name_str);
                                    string_free(pw_str);
                                    exit = true;
                                }
                            }
                        }
                        break;

                    default:
                        string_concat(name_str, &tmp);
                        break;
                }
            } else {
                string_free(name_str);
                string_free(pw_str);
                exit = true;
            }
        }
        fclose(pw_list);
        return false;
    }
    return false;
}

/*
 * bool read_pw_list(Hash* hash){
    string * pw_list_pfad_st = pw_rood();
    string * pw_list_pfad = string_terminate(pw_list_pfad_st);

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
                    free(name_str->buf);
                    name_str->len = 0;
                    break;
                case ':': // :
                    if (string_compare(hash->key,name_str)){

                        while ((tmp = fgetc(pw_list)) != '}' && exit != true){
                            if (tmp == EOF){
                                exit = true;
                            }
                        }
                        while ((tmp = fgetc(pw_list)) != '=' && exit != true){
                            if (tmp == EOF){
                                exit = true;
                            } else {
                                string_concat(pw_str, &tmp);
                            }
                        }

                        if (string_compare(hash->value,pw_str)){
                            fclose(pw_list);
                            string_free(name_str);
                            string_free(pw_str);
                            return true;
                        } else {
                            fclose(pw_list);
                            string_free(name_str);
                            string_free(pw_str);
                            return false;
                        }
                    } else {
                        if (name_str->len != 0){
                            free(name_str->buf);
                            name_str->len = 0;
                        }
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
        string_free(name_str);
        string_free(pw_str);
        fclose(pw_list);
        return false;
    }
    return false;
}
 */