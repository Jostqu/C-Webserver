//
// Created by bob on 28.05.19.
//

#include "Test.h"
#include "helpers.h"
#include "hash.h"
#include "request.h"
#include "response.h"

void test_hashlist(){

    string* s1 = string_new_from_cstr("Pepsi ist besser als");
    string* s2 = string_new_from_cstr("coca cola");
    string* s3 = string_new_from_cstr("Hummeln sind");
    string* s4 = string_new_from_cstr("Bienen");
    string* s5 = string_new_from_cstr("Henning fand es");
    string* s6 = string_new_from_cstr("lustig");
    string* s7 = string_new_from_cstr("Ctype from Destop/LMAO: ");
    string* s8 = get_content_type("/home/bjoern/Desktop/LMAO");

    HashList* list = SHL_create(SH_create(s1,s2));
    SHL_append(list, SH_create(s3, s4));
    SHL_append(list, SH_create(s5, s6));
    SHL_append(list, SH_create(s7, s8));

    int i = SHL_get_size(list);

    printf("%d", i);

    for (int j = 0; j < i; ++j) {
        string_print(SHL_at(list, j).key);
        string_print(SHL_at(list, j).value);
    }

    SHL_remove_all(list);
}

//this seems to be buggy, but im too tired to fix it now
void test_response_build(){

    string* s1 = string_new(15);
    string* s2 = string_new(15);
    string* s3 = string_new(15);
    string* s4 = string_new(15);

    string_concat(s1, "Name");
    string_concat(s2, "surwer");
    string_concat(s3, "yabba");
    string_concat(s4, "dabbadooh");

    HashList* list = SHL_create(SH_create(s1,s2));
    SHL_append(list, SH_create(s3,s4));

    string *s = build_http_response_header(OK, list);

    string_print(s);

    string_free(s);
    SHL_remove_all(list);
}

void test_string_split(){
    string* str = string_new(10);
    string_concat(str, "SPLITBY:");

    int splits;
    string** splitted = string_split(str, ':', &splits);

    printf("%d", splits);

    for (int i = 0; i < splits; ++i) {
        printf("String %d:", i);
        string_print(splitted[i]);
    }

    string_free(str);
    free(splitted);
}

void test_get_ctype() {
    string* type = get_content_type("/home/bjoern/Desktop/LMAO");

    if(type) {
        string_print(type);
        string_free(type);
    }
}

void test_string_insert(){
    string* dst = string_new_from_cstr("Ha Welt");
    string* src = string_new_from_cstr("lo");

    string_insert_cstr(src, "l", 1);
    string_insert(dst, src, 2);

    string_print(dst);

    string_free(dst);
    string_free(src);
}

void test_string_split_string(){
    string* str = string_new_from_cstr("GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close");

    int splits;

    string** arr = string_split_cstr(str, "\r\n", &splits);

    printf("%d\n", splits);

    for (int i = 0; i < splits; ++i) {
        string_print(arr[i]);
        string_free(arr[i]);
    }

    string_free(str);
    free(arr);
}