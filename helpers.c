#include "helpers.h"

//thanks to "man popen" and marcel for the idea of using it
string *get_content_type(char *path) {
    if(!path)
        return NULL;

    if(strlen(path) > 1024){
        perror("path too long, shorten it to 1024 digits or less");
        return NULL;
    }

    //1024 chars max path length + 8 chars command ("file -i ") + terminating 0 = 1033
    char command[1033] = "file -i ";
    strcat(command, path);

    FILE* f = popen(command, "r");

    if(!f)
        return NULL;

    char buffer[128];
    memset(buffer, 0, 128);
    fread(buffer, 128, 128, f);

    fclose(f);

    //note, this seems to not work for some weird reason
    //it is supposed be triggered if the file does not exist so the path would be
    //something like /../folder/lol.jpg and buffer[0] should be 'c' from the "cannot open" message
    //so for not its needed to bechecked after the fuction returned wether the file was evaluated or not
    if(path[0] != buffer[0])
        return NULL;

    string* b = string_new(10);
    string_concat(b, buffer);

    int splits;
    string** list = string_split(b, ':', &splits);

    string* ret = list[1];

    string_strip(ret);

    string_free(b);
    string_free(list[0]);
    free(list);

    return ret;
}
