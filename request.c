//
// Created by Marcel on 26.04.2019.
//

#include "request.h"

#define METHOD_CAPACITY 7
#define PATH_CAPACITY 2000
#define VERSION_CAPACITY 8
#define KEY_CAPACITY 50
#define VALUE_CAPACITY 100

void free_http_request(HttpRequest* httpRequest)
{
	if (httpRequest->path)
		string_free(httpRequest->path);

	if (httpRequest->fields)
        SHL_remove_all(httpRequest->fields);

	free(httpRequest);
}

//
// Request-Aufbau
// --------------
// METHOD path HTTP/VERSION\r\n
// key: value\r\n
// ...
// \r\n
// \r\n
// Daten
//
HttpResponseCodes parse_http_request(void* buffer, size_t bufferSize, HttpRequest* httpRequest)
{
    HttpRequestParsingState parsingState = PARSING_METHOD;

    string* strMethod = string_new(METHOD_CAPACITY);
    string* strPath = string_new(PATH_CAPACITY);
    string* strVersion = string_new(VERSION_CAPACITY);
    string* strKey = NULL;
    string* strValue = NULL;

    httpRequest->path = NULL;
    httpRequest->fields = NULL;
    httpRequest->data = NULL;

    // Zeichenweise durch den Anfrage-Puffer gehen
    for (size_t i = 0; i < bufferSize && httpRequest->data == NULL; i++)
    {
        // Aktuelles Zeichen
        char c = ((char*)(buffer))[i];
        if (c == '\r') // Backspaces überspringen
            continue;

        switch (parsingState)
        {
            case PARSING_METHOD:
                if (c != ' ')
                {
                    // Wenn kein Leerzeichen, dann Zeichen an den String anhängen
                    string_add_char(strMethod, c);
                }
                else
                {
                    httpRequest->method = get_method_from_string(strMethod);
                    // Wenn ein Leerzeichen gefunden wurde, den Pfad parsen
                    parsingState = PARSING_PATH;
                }
                break;

            case PARSING_PATH:
                if (c != ' ')
                {
                    // Wenn kein Leerzeichen, dann Zeichen an den String anhängen
                    string_add_char(strPath, c);
                }
                else
                {
                    url_decode(strPath);
                    // Wenn ein Leerzeichen gefunden wurde, die Version parsen
                    parsingState = PARSING_VERSION;
                }
                break;

            case PARSING_VERSION:
                if (c != '\n')
                {
                    // Wenn keine neue Zeile, dann Zeichen an den String anhängen
                    string_add_char(strVersion, c);
                }
                else
                {
                    httpRequest->version = validate_version(strVersion);
                    // Wenn eine neue Zeile beginnt, Key eines Feldes parsen
                    parsingState = PARSING_FIELD_KEY;
                }
                break;

            case PARSING_FIELD_KEY:
                if (c == '\n')
                {
                    // Wenn beim Parsen des Keys eine neue Zeile gefunden wird, sind die Felder zu Ende und es folgen die Daten
                    parsingState = PARSING_DATA;
                }
                else if (c != ':')
                {
                	if (!strKey)
	                {
                		strKey = string_new(KEY_CAPACITY);
	                }

                    // Wenn kein Doppelpunkt, dann Zeichen an den String anhängen
                    string_add_char(strKey, tolower(c));
                }
                else
                {
                    // TODO: Key ggf. validieren

                    // Wenn ein Doppelpunkt gefunden wurde, den Value parsen
                    parsingState = PARSING_FIELD_VALUE;
                }
                break;

            case PARSING_FIELD_VALUE:
                if (c != '\n')
                {
                	if (!strValue)
	                {
                		strValue = string_new(VALUE_CAPACITY);
	                }

                	// Wenn keine neue Zeile, dann Zeichen an den String anhängen
                    string_add_char(strValue, c);
                }
                else
                {
                    strValue = string_strip(strValue);

                    // strKey und strValue werden am Ende von der Hashmap freigegeben
                    Hash pair = SH_create(strKey, strValue);
                    if (httpRequest->fields == NULL)
                    {
                        httpRequest->fields = SHL_create(pair);
                    }
                    else
                    {
                        SHL_append(httpRequest->fields, pair);
                    }

                    strKey = NULL;
                    strValue = NULL;

                    // Wenn eine neue Zeile anfängt, wieder den Key parsen
                    parsingState = PARSING_FIELD_KEY;
                }
                break;

            case PARSING_DATA:
                httpRequest->data = buffer + i;
                break;

            default:
                break;
        }
    }

	// TODO: Pfad validieren. Referer-Key beachten! (Marcel)
	string* validatedPath = string_new(PATH_CAPACITY);
    string_concat(validatedPath, "../htdocs");
    string_concat_str(validatedPath, strPath);
	httpRequest->path = validatedPath;

	string_free(strVersion);
	string_free(strPath);
    string_free(strMethod);

    return OK;
}
