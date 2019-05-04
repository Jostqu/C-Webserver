#include "response.h"

#define SERVER_NAME "PSE HTTP-Server v1.0"
#define FILE_BUFFER_SIZE 65536 // 64 kB

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

string *build_http_response_header(HttpResponseCodes code, HashList *fields) {

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

    //add separating empty line
	string_add_char(response, '\r');
	string_add_char(response, '\n');

    return response;
}

void write_buffer_to_socket(int socket, char* buffer, size_t len)
{
	if (write(socket, buffer, len) < 0)
	{
		perror("write_buffer_to_socket: failed to write to socket!");
		exit(1);
	}
}

void write_string_to_socket(int socket, string* str)
{
	write_buffer_to_socket(socket, str->buf, str->len);
}

void send_http_response(int targetStream, HttpResponseCodes code, string *path)
{
	string* strServerKey = string_new_from_cstr("Server");
	string* strServerValue = string_new_from_cstr(SERVER_NAME);
	HashList* fields = SHL_create(SH_create(strServerKey, strServerValue));

	string* strContentTypeKey = string_new_from_cstr("Content-Type");
	string* strContentTypeValue;

	if (code == OK)
	{
		string_terminate(path);

		strContentTypeValue = get_content_type(path->buf);
		SHL_append(fields, SH_create(strContentTypeKey, strContentTypeValue));

		FILE* fp = fopen(path->buf, "rb");
		if (fp)
		{
			string* strContentLengthKey = string_new_from_cstr("Content-Length");
			string* strContentLengthValue = int_to_string(get_file_size(fp));
			SHL_append(fields, SH_create(strContentLengthKey, strContentLengthValue));

			// Antwort erstellen und versenden
			string* strResponse = build_http_response_header(code, fields);
			write_string_to_socket(targetStream, strResponse);
			string_free(strResponse);

			// Datei senden
			size_t length = 0;
			char fileBuffer[FILE_BUFFER_SIZE];
			while ((length = fread(fileBuffer, 1, sizeof(fileBuffer), fp)) > 0)
			{
				write_buffer_to_socket(targetStream, fileBuffer, length);
			}

			fclose(fp);
		}
		else
		{
			// Dürfte nie ausgeführt werden
			fprintf(stderr, "File '%s' not found!\n", path->buf);
		}
	}
	else
	{
		strContentTypeValue = string_new_from_cstr("text/plain");
		SHL_append(fields, SH_create(strContentTypeKey, strContentTypeValue));

		string* strContent = string_new_from_cstr(code_to_string(code));
		string* strResponse = build_http_response_header(code, fields);
		string_concat_str(strResponse, strContent);
		string_free(strContent);

		// Antwort senden
		write_string_to_socket(targetStream, strResponse);

		string_free(strResponse);
	}

	SHL_remove_all(fields);
}
