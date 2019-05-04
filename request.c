//
// Created by Marcel on 26.04.2019.
//

#include "request.h"

#define METHOD_CAPACITY 7
#define PATH_CAPACITY 2083
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

HttpResponseCodes validate_path(string *path, string** validatedPath)
{
	string* htdocsDir = string_new(PATH_CAPACITY);
	getcwd(htdocsDir->buf, PATH_CAPACITY);
	htdocsDir->len = strlen(htdocsDir->buf);

#ifndef NDEBUG
	// Ein Verzeichnis zurückgehen, da wir uns beim Debug-Build im cmake-build-debug Ordner befinden
	string_concat(htdocsDir, "/..");
#endif

	string_concat(htdocsDir, "/htdocs");
	string_terminate(htdocsDir);

	string* absoluteHtdocsDir = string_new(PATH_CAPACITY);
	realpath(htdocsDir->buf, absoluteHtdocsDir->buf);
	absoluteHtdocsDir->len = strlen(absoluteHtdocsDir->buf);

	string_free(htdocsDir);

	string* tmp = string_new(PATH_CAPACITY);
	string_concat_str(tmp, absoluteHtdocsDir);
	string_concat_str(tmp, path);

	if (!isfile(tmp))
	{
		string_concat(tmp, "/index.html");
	}

	string_terminate(tmp);

	string* absolutePath = string_new(PATH_CAPACITY);
	realpath(tmp->buf, absolutePath->buf);
	absolutePath->len = strlen(absolutePath->buf);

	string_free(tmp);

	HttpResponseCodes responseCode;
	if (absolutePath->len < absoluteHtdocsDir->len || memcmp(absolutePath->buf, absoluteHtdocsDir->buf, absoluteHtdocsDir->len) < 0) // TODO: Oder Datei darf nicht zurückgegeben werden z.B. htpasswd
	{
		string_free(absolutePath);
		responseCode = FORBIDDEN;
	}
	else
	{
		if (file_exists(absolutePath))
		{
			*validatedPath = absolutePath;
			responseCode = OK;
		}
		else
		{
			string_free(absolutePath);
			responseCode = NOT_FOUND;
		}
	}

	string_free(absoluteHtdocsDir);
	return responseCode;
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
HttpResponseCodes parse_http_request(char* buffer, size_t bufferSize, HttpRequest* httpRequest)
{
	if (bufferSize == 0)
		return BAD_REQUEST;

    HttpRequestParsingState parsingState = PARSING_METHOD;
    HttpResponseCodes responseCode = OK;

    string* strMethod = string_new(METHOD_CAPACITY);
    string* strPath = NULL;
    string* strVersion = string_new(VERSION_CAPACITY);
    string* strKey = NULL;
    string* strValue = NULL;

    httpRequest->path = NULL;
    httpRequest->fields = NULL;
    httpRequest->data = NULL;

    size_t i;

    // Zeichenweise durch den Anfrage-Puffer gehen
    for (i = 0; i < bufferSize && httpRequest->data == NULL && responseCode == OK; i++)
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
                	if (!strPath)
	                {
		                strPath = string_new(PATH_CAPACITY);
	                }

                    // Wenn kein Leerzeichen, dann Zeichen an den String anhängen
                    string_add_char(strPath, c);

                	if (strPath->len > PATH_CAPACITY)
	                {
                		responseCode = BAD_REQUEST;
	                }
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
                    if (httpRequest->version == UNSUPPORTED)
                    {
                    	responseCode = BAD_REQUEST;
                    }

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

//    string* strReferer = string_new(10);
//    string_concat(strReferer, "referer");
//    Hash hash = SHL_find_key(httpRequest->fields, strReferer);
//    if (&hash)
//    {
//    	int splits;
//    	string** refererParts = string_split(hash.value, '/', &splits);
//
////    	string* newStrPath = string_new(PATH_CAPACITY);
////    	string_concat_str(newStrPath, );
////    	string_concat_str(newStrPath, strPath);
//
//		for (int x = 0; x < splits; x++)
//			string_free(refererParts[x]);
//    }
//    string_free(strReferer);

	string* validatedPath = NULL;
	responseCode = validate_path(strPath, &validatedPath);
	string_free(strPath);

	if (responseCode == OK)
	{
		httpRequest->path = validatedPath;
	}

	string_free(strVersion);
    string_free(strMethod);

    return responseCode;
}
