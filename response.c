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

void send_http_response(int targetStream, HttpResponseCodes code, string *path)
{
	string* strServerKey = string_new(6);
	string_concat(strServerKey, "Server");

	string* strServerValue = string_new(50);
	string_concat(strServerValue, SERVER_NAME);

	HashList* fields = SHL_create(SH_create(strServerKey, strServerValue));

	if (code == OK)
	{
		string_terminate(path);

		string* strContentTypeKey = string_new(20);
		string_concat(strContentTypeKey, "Content-Type");

		string* strContentTypeValue = get_content_type(path->buf);

		SHL_append(fields, SH_create(strContentTypeKey, strContentTypeValue));

		FILE* fp = fopen(path->buf, "rb");
		if (fp)
		{
			// Für die Dateigröße, an das Ende springen und Position lesen
			fseek(fp, 0, SEEK_END);
			long fileSize = ftell(fp);

			string* strFileSizeKey = string_new(20);
			string_concat(strFileSizeKey, "Content-Length");
			string* strFileSizeValue = int_to_string(fileSize);

			SHL_append(fields, SH_create(strFileSizeKey, strFileSizeValue));

			string* strResponse = build_http_response_header(code, fields);

			// Antwort-Header senden
			if (write(targetStream, strResponse->buf, strResponse->len) < 0)
			{
				perror("ERROR writing to stream!");
				exit(1);
			}

			// Wieder an den Anfang der Datei springen
			fseek(fp, 0, SEEK_SET);

			// Daten senden
			size_t length = 0;
			char fileBuffer[FILE_BUFFER_SIZE];
			while ((length = fread(fileBuffer, 1, sizeof(fileBuffer), fp)) > 0)
			{
				if (write(targetStream, fileBuffer, length) < 0)
				{
					perror("ERROR writing to stream!");
					break;
				}
			}

			string_free(strResponse);
			fclose(fp);
		}
		else
		{
			fprintf(stderr, "File '%s' not found!\n", path->buf);
		}
	}
	else
	{
		string* strContentTypeKey = string_new(20);
		string_concat(strContentTypeKey, "Content-Type");

		string* strContentTypeValue = string_new(30);
		string_concat(strContentTypeValue, "text/plain");

		SHL_append(fields, SH_create(strContentTypeKey, strContentTypeValue));

		string* strContent = string_new(30);
		string_concat(strContent, code_to_string(code));

		string* strResponse = build_http_response_header(code, fields);
		string_concat_str(strResponse, strContent);

		// Antwort senden
		if (write(targetStream, strResponse->buf, strResponse->len) < 0)
		{
			perror("ERROR writing to stream!");
			exit(1);
		}

		string_free(strResponse);
		string_free(strContent);
	}

	SHL_remove_all(fields);
}
