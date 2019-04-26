#include "response.h"

string *code_to_string(HttpResponseCodes code) {

    string* b;

    switch (code){

        case OK:{
            b = string_new(15);
            string_concat(b, "200 OK");
            break;
        }
        case BAD_REQUEST:{
            b = string_new(15);
            string_concat(b, "400 Bad Request");
            break;
        }

        case UNAUTHORIZED:{
            b = string_new(15);
            string_concat(b, "401 Unauthorized");
            break;
        }

        case FORBIDDEN:{
            b = string_new(15);
            string_concat(b, "403 Forbidden");
            break;
        }

        case NOT_FOUND:{
            b = string_new(15);
            string_concat(b, "404 Not Found");
            break;
        }

        case METHOD_NOT_ALLOWED:{
            b = string_new(20);
            string_concat(b, "405 Method Not Allowed");
            break;
        }

        default:
            b = NULL; //just to make sure it really is null
            break;
    }

    return b;
}

string *build_http_response(HttpResponseCodes code, HashList *fields, string *body) {

    string* strCode = code_to_string(code);
    if(!strCode)
        return NULL;

    string* response = string_new(1000);

    //add HTTP version and response code
    string_concat(response, "HTTP/1.1 ");
    string_concat_str(response, strCode);
    string_add_char(response, '\r');
    string_add_char(response, '\n');

    //add custom fields
    int n = SHL_get_size(fields);
    for (int i = 0; i < n; i++) {
        string_concat_str(response, SHL_at(fields, i).key);
        string_concat(response, ": ");
        string_concat_str(response, SHL_at(fields, i).value);
        string_add_char(response, '\r');
        string_add_char(response, '\n');
    }

    //add seperating empty line
    string_add_char(response, '\r');
    string_add_char(response, '\n');
    string_concat_str(response, body);

    string_free(strCode);

    return response;
}
