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
    // Passwort geprinte, sollte demnächst wieder entfernt werden
    string_print(st_encode);

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


bool authorizaition (HashList * hashlist){
    int temp = abfrage_authorizaition(hashlist);
    if (temp == true){
        Hash  passwort = password_to_sha1Base64(hashlist);
        Hash *str = SHL_find_key_cstr(hashlist, "authorization");
        printf("Name: ");
        string_print(passwort.key);
        printf("Password: ");
        string_print(passwort.value);
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

bool file_char_into_str(FILE *file,string *str,char delimiter){
    char tmp;
    bool eof = false;

    while ((tmp = fgetc(file)) != delimiter && eof != true) {
        if (tmp == EOF) {
            eof = true;
        } else {
            string_concat(str, &tmp);
        }
    }
    return eof;
}

bool file_skip(FILE *file,char delimiter){
    char tmp;
    bool eof = false;

    while ((tmp = fgetc(file)) != delimiter && eof != true) {
        if (tmp == EOF) {
            eof = true;
        }
    }
    return eof;
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

        string *name_str = string_new(1024);
        string *pw_str = string_new(1024);
        while(exit != true){
            if ((tmp = fgetc(pw_list))!= EOF) {
                switch (tmp) {
                    case '\n': // New Line
                        string_free(name_str);
                        break;
                    case ':': // :
                        if (string_compare(hash->key, name_str)) {

                                exit = file_skip(pw_list,'}');

                                if (exit != true){
                                    file_char_into_str(pw_list,pw_str ,'\n');
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

                            exit = file_skip(pw_list,'\n');
                        }
                        break;

                    default:
                        string_concat(name_str, &tmp);
                        break;
                }
            } else {
                exit = true;
            }
        }
        string_free(name_str);
        string_free(pw_str);
        fclose(pw_list);
        return false;
    }
    return false;
}

