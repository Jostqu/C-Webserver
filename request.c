//
// Created by Marcel on 26.04.2019.
//

#include "request.h"

#define METHOD_CAPACITY 7
#define PATH_CAPACITY 2083
#define PATH_CAPACITY_ABSOLUTE (PATH_CAPACITY + 200)
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
	string* htdocsDir = string_new(PATH_CAPACITY_ABSOLUTE);
	getcwd(htdocsDir->buf, PATH_CAPACITY_ABSOLUTE);
	htdocsDir->len = strlen(htdocsDir->buf);

#ifndef NDEBUG
	// Ein Verzeichnis zurückgehen, da wir uns beim Debug-Build im cmake-build-debug Ordner befinden
	string_concat(htdocsDir, "/..");
#endif

	string_concat(htdocsDir, "/htdocs");
	string_terminate(htdocsDir);

	string* absoluteHtdocsDir = string_new(PATH_CAPACITY_ABSOLUTE);
	realpath(htdocsDir->buf, absoluteHtdocsDir->buf);
	absoluteHtdocsDir->len = strlen(absoluteHtdocsDir->buf);

	string_free(htdocsDir);

	string* tmp = string_new(PATH_CAPACITY_ABSOLUTE);
	string_concat_str(tmp, absoluteHtdocsDir);
	string_concat_str(tmp, path);

	if (!isfile(tmp))
	{
		string_concat(tmp, "/index.html");
	}

	string_terminate(tmp);

	string* absolutePath = string_new(PATH_CAPACITY_ABSOLUTE);
	realpath(tmp->buf, absolutePath->buf);
	absolutePath->len = strlen(absolutePath->buf);

	string_print(absolutePath);

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

HttpRequestParsingState parsing_method(char c, string* strMethod, HttpRequest** httpRequest, HttpResponseCodes* responseCode)
{
	if (c != ' ')
	{
		// Wenn kein Leerzeichen, dann Zeichen an den String anhängen
		string_add_char(strMethod, c);

		// Weiterhin die Methode parsen
		return PARSING_METHOD;
	}
	else
	{
		(*httpRequest)->method = get_method_from_string(strMethod);
		if ((*httpRequest)->method != GET)
		{
			*responseCode = NOT_IMPLEMENTED;
		}

		// Wenn ein Leerzeichen gefunden wurde, den Pfad parsen
		return PARSING_PATH;
	}
}

HttpRequestParsingState parsing_path(char c, string** strPath, HttpRequest** httpRequest, HttpResponseCodes* responseCode)
{
	if (c != ' ')
	{
		if (!(*strPath))
		{
			*strPath = string_new(PATH_CAPACITY);
		}

		// Wenn kein Leerzeichen, dann Zeichen an den String anhängen
		string_add_char(*strPath, c);

		if ((*strPath)->len > PATH_CAPACITY)
		{
			*responseCode = BAD_REQUEST;
		}

		// Weiterhin den Pfad parsen
		return PARSING_PATH;
	}
	else
	{
		url_decode(*strPath);

		// Wenn ein Leerzeichen gefunden wurde, die Version parsen
		return PARSING_VERSION;
	}
}

HttpRequestParsingState parsing_version(char c, string* strVersion, HttpRequest** httpRequest, HttpResponseCodes* responseCode)
{
	if (c != '\n')
	{
		// Wenn keine neue Zeile, dann Zeichen an den String anhängen
		string_add_char(strVersion, c);

		// Weiterhin die Version parsen
		return PARSING_VERSION;
	}
	else
	{
		(*httpRequest)->version = validate_version(strVersion);
		if ((*httpRequest)->version == UNSUPPORTED)
		{
			*responseCode = BAD_REQUEST;
		}

		// Wenn eine neue Zeile beginnt, Key eines Feldes parsen
		return PARSING_FIELD_KEY;
	}
}

HttpRequestParsingState parsing_field_key(char c, string** strKey)
{
	if (c == '\n')
	{
		// Wenn beim Parsen des Keys eine neue Zeile gefunden wird, sind die Felder zu Ende und es folgen die Daten
		return PARSING_DATA;
	}
	else if (c != ':')
	{
		if (!(*strKey))
		{
			*strKey = string_new(KEY_CAPACITY);
		}

		// Wenn kein Doppelpunkt, dann Zeichen an den String anhängen
		string_add_char(*strKey, tolower(c));

		// Weiterhin den Key des Feldes parsen
		return PARSING_FIELD_KEY;
	}
	else
	{
		// Wenn ein Doppelpunkt gefunden wurde, den Value parsen
		return PARSING_FIELD_VALUE;
	}
}

HttpRequestParsingState parsing_field_value(char c, string** strKey, string** strValue, HttpRequest** httpRequest)
{
	if (c != '\n')
	{
		if (!(*strValue))
		{
			*strValue = string_new(VALUE_CAPACITY);
		}

		// Wenn keine neue Zeile, dann Zeichen an den String anhängen
		string_add_char(*strValue, c);

		// Weiterhin den Value des Feldes parsen
		return PARSING_FIELD_VALUE;
	}
	else
	{
		string_strip(*strValue);

		// strKey und strValue werden am Ende von der Hashmap freigegeben
		Hash pair = SH_create(*strKey, *strValue);
		if ((*httpRequest)->fields == NULL)
		{
			(*httpRequest)->fields = SHL_create(pair);
		}
		else
		{
			SHL_append((*httpRequest)->fields, pair);
		}

		*strKey = NULL;
		*strValue = NULL;

		// Wenn eine neue Zeile anfängt, wieder den Key parsen
		return PARSING_FIELD_KEY;
	}
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
HttpResponseCodes parse_http_request(char* buffer, size_t bufferSize, HttpRequest* httpRequest, string** staticPage)
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
                parsingState = parsing_method(c, strMethod, &httpRequest, &responseCode);
                break;

            case PARSING_PATH:
            	parsingState = parsing_path(c, &strPath, &httpRequest, &responseCode);
                break;

            case PARSING_VERSION:
            	parsingState = parsing_version(c, strVersion, &httpRequest, &responseCode);
                break;

            case PARSING_FIELD_KEY:
            	parsingState = parsing_field_key(c, &strKey);
                break;

            case PARSING_FIELD_VALUE:
            	parsingState = parsing_field_value(c, &strKey, &strValue, &httpRequest);
                break;

            case PARSING_DATA:
                httpRequest->data = buffer + i;
                break;

            default:
                break;
        }
    }

//    if (httpRequest->fields)
//    {
//	    Hash *hash = SHL_find_key_cstr(httpRequest->fields, "referer");
//	    if (hash)
//	    {
//		    int splits;
//		    string **refererParts = string_split(hash->value, '/', &splits);
//		    for (int x = 0; x < splits; x++)
//		    {
//		    	string_print(refererParts[x]);
//		    }
//
//		    if (splits > 0)
//		    {
//    	        string* newStrPath = string_new(PATH_CAPACITY);
//    	        string_concat_str(newStrPath, refererParts[splits-1]);
//    	        string_concat_str(newStrPath, strPath);
//    	        string_free(strPath);
//    	        strPath = newStrPath;
//
//			    for (int x = 0; x < splits; x++)
//				    string_free(refererParts[x]);
//		    }
//	    }
//    }

	if (responseCode == OK)
	{
		if (string_compare_cstr(strPath, "/debug"))
		{
			*staticPage = string_new(200);

			string_concat(*staticPage, "Methode: ");
			string_concat_str(*staticPage, strMethod);
			string_concat(*staticPage, "\n");

			string_concat(*staticPage, "Ressource: ");
			string_concat_str(*staticPage, strPath);
			string_concat(*staticPage, "\n");

			string_concat(*staticPage, "Version: ");
			string_concat_str(*staticPage, strVersion);
			string_concat(*staticPage, "\n");
		}
		else
		{
			string* validatedPath = NULL;
			responseCode = validate_path(strPath, &validatedPath);

			if (responseCode == OK)
			{
				httpRequest->path = validatedPath;
			}
		}
	}

	string_free(strPath);
	string_free(strVersion);
    string_free(strMethod);

    return responseCode;
}
