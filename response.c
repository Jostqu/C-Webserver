#include "response.h"

char *code_to_string(HttpResponseCodes code) {

    char* b = NULL;

    switch (code){

        case OK:{
            b = "200 OK";
            break;
        }
        case BAD_REQUEST:{
            b = "400 Bad Request";
            break;
        }

        case UNAUTHORIZED:{
            b = "401 Unauthorized";
            break;
        }

        case FORBIDDEN:{
            b = "403 Forbidden";
            break;
        }

        case NOT_FOUND:{
            b = "404 Not Found";
            break;
        }

        case METHOD_NOT_ALLOWED:{
            b = "405 Method Not Allowed";
            break;
        }

        default:
            break;
    }

    return b;
}

string *build_http_response(HttpResponseCodes code, HashList *fields, string *body) {

    char* strCode = code_to_string(code);
    if(!strCode)
        return NULL;

    string* response = 0;

    while (!response)
        response = string_new(1000);

    //add HTTP version and response code
    string_concat(response, "HTTP/1.1 ");
    string_concat(response, strCode);
    string_add_char(response, '\r');
    string_add_char(response, '\n');

    //add custom fields
    if(fields) {
        int n = SHL_get_size(fields);
        for (int i = 0; i < n; i++) {
            string_concat_str(response, SHL_at(fields, i).key);
            string_concat(response, ": ");
            string_concat_str(response, SHL_at(fields, i).value);
            string_add_char(response, '\r');
            string_add_char(response, '\n');
        }
    }

    if(body) {
        string* bodyLength = int_to_string(body->len);

        //add content length field
        string_concat(response, "Content-Length: ");
        string_concat_str(response, bodyLength);
        string_add_char(response, '\r');
        string_add_char(response, '\n');

        string_free(bodyLength);

        //add seperating empty line
        string_add_char(response, '\r');
        string_add_char(response, '\n');

        string_concat_str(response, body);
    }

    return response;
}
