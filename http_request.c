//
// Created by Marcel on 26.04.2019.
//

#include "http_request.h"

void free_http_request(HttpRequest httpRequest)
{
    SHL_remove_all(httpRequest.fields);
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
int parse_http_request(void* buffer, size_t bufferSize, HttpRequest* httpRequest)
{
    HttpRequestParsingState parsingState = PARSING_METHOD;

    string* strMethod = string_new(10);
    string* strPath = string_new(2500);
    string* strVersion = string_new(10);
    string* strKey = string_new(30);
    string* strValue = string_new(50);

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
                    // TODO: Methode validieren und in httpRequest speichern (Leon)

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
                    // TODO: Url decoden (Leon)
                    // TODO: Pfad validieren und in httpRequest speichern (Marcel)

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
                    // TODO: Version validieren und in httpRequest speichern (Leon)

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
                    // Wenn keine neue Zeile, dann Zeichen an den String anhängen
                    string_add_char(strValue, c);
                }
                else
                {
                    strValue = string_strip(strValue);

                    Hash pair = SH_create(*strKey, *strValue);
                    if (httpRequest->fields == NULL)
                    {
                        httpRequest->fields = SHL_create(pair);
                    }
                    else
                    {
                        SHL_append(httpRequest->fields, pair);
                    }

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

    string_free(strVersion);
    string_free(strPath);
    string_free(strMethod);

    // OK
    return 200;
}
