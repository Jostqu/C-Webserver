#include "helpers.h"
#include "hash.h"

//thanks to "man popen" and marcel for the idea of using it
string *get_content_type(char *path) {
    if(!path)
        return NULL;

    if(strlen(path) > 3072){
        perror("path too long, shorten it to 1024 digits or less");
        return NULL;
    }

    //3072 chars max path length + 10 chars command and '' ("file -i '<path>'") + terminating 0 = 3081
    char command[3081] = "file -i \"";
    strcat(command, path);
    strcat(command, "\"");

    FILE* f = popen(command, "r");

    if(!f)
        return NULL;

    char buffer[3200];
    memset(buffer, 0, 3200);
    fread(buffer, 3200, 3200, f);

    fclose(f);

    if(path[0] != buffer[0])
        return NULL;

    string* b = string_new(3200);
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

void url_decode(string *str) {
    if(!str)
        return;

    string* numAsString = string_new(3);

    for (int i = 0; i < str->len; ++i) {
        if(str->buf[i] == '%'){

            string_add_char(numAsString, str->buf[i+1]);
            string_add_char(numAsString, str->buf[i+2]);
            string_terminate(numAsString);

            str->buf[i] = string_to_long(numAsString, 16);
            memmove(str->buf+i+1, str->buf+i+3, str->len-i-2);
            str->len -= 2;

            numAsString->len = 0;
        }
    }

    string_free(numAsString);
}

HTTPVersion validate_version(string *ver) {
    if(string_compare_cstr(ver, "HTTP/0.9") || string_compare_cstr(ver, "HTTP/1.0") || string_compare_cstr(ver, "HTTP/1.1"))
        return VERSION_1_1;
    else
        return UNSUPPORTED;
}

HTTPMethod get_method_from_string(string *method) {
    if(string_compare_cstr(method, "GET"))
        return GET;
    else if(string_compare_cstr(method, "POST"))
        return POST;
    else if (string_startswith_cstr(method, "HEAD"))
    	return HEAD;
    else if (string_startswith_cstr(method, "PUT"))
    	return PUT;
    else if (string_startswith_cstr(method, "PATCH"))
	    return PATCH;
    else if (string_startswith_cstr(method, "DELETE"))
	    return DELETE;
    else if (string_startswith_cstr(method, "TRACE"))
	    return TRACE;
    else if (string_startswith_cstr(method, "OPTIONS"))
	    return OPTIONS;
    else if (string_startswith_cstr(method, "CONNECT"))
	    return CONNECT;
    else
        return INVALID;
}

bool isfile(string *path)
{
	string_terminate(path);
	struct stat s;
	s.st_mode = 0;
	stat(path->buf, &s);
	return S_ISREG(s.st_mode);
}

bool file_exists(string *path)
{
	string_terminate(path);
	struct stat s;
	return stat(path->buf, &s) == 0;
}

long get_file_size(FILE *fp)
{
	// Für die Dateigröße, an das Ende springen und Position lesen
	fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);

	// Wieder an den Anfang der Datei springen
	fseek(fp, 0, SEEK_SET);

	return fileSize;
}

