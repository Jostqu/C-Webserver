#include "helpers.h"

string *get_content_type(char *path) {
    if(!path)
        return NULL;

    //127 chars path lengh + 8 chars command ("file -i ") + binary 0 = 136
    char command[136] = "file -i ";
    strcat(command, path);

    FILE* f = popen(command, "r");

    if(!f)
        return NULL;

    char buffer[128];
    fread(buffer, 128, 128, f);

    fclose(f);

    string* b = string_new(10);
    string_concat(b, buffer);

    int splits;
    string** list = string_split(b, ':', &splits);

    string* ret = list[1];

    string_free(b);
    string_free(list[0]);
    free(list);

    return ret;
}
