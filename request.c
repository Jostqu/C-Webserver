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

static bool debugPage = false;
static HttpResponseCodes methodCode = OK;

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
	// going back one directory (working dir is in cmake-build-debug in debug-configuration)
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

	// causes 'conditional jump or move depends on uninitialised value' warning
	if (!isfile(tmp))
	{
		string_concat(tmp, "/index.html");
	}

	string_terminate(tmp);

	string* absolutePath = string_new(PATH_CAPACITY_ABSOLUTE);
	realpath(tmp->buf, absolutePath->buf);
	absolutePath->len = strlen(absolutePath->buf);

//	string_print(absolutePath);

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
			methodCode = NOT_IMPLEMENTED;
			*responseCode = OK; // liar
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

		if (!string_startswith_cstr(*strPath, "/debug"))
		{
			*responseCode = methodCode;
		}
		else
		{
			debugPage = true;
		}

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

static string* get_referer_path(HashList* fields)
{
	string* refererPath = NULL;
	if (fields)
	{
		// check if referer-field exists
		Hash *hash = SHL_find_key_cstr(fields, "referer");
		if (hash)
		{
			// get relative resource path from referer-value
			int splits;
			string **refererParts = string_split(hash->value, '/', &splits);
			if (splits > 3)
			{
				// only take parts after domain name and rejoin them
				refererPath = string_join(refererParts + 3, splits - 3, '/');
				string_insert_cstr(refererPath, "/", 0);

				if (string_endswith_cstr(refererPath, "index.html"))
				{
					string_free(refererPath);
					refererPath = NULL;
				}
			}

			// free all parts from string_split
			for (int x = 0; x < splits; x++)
				string_free(refererParts[x]);
			free(refererParts);
		}
	}
	return refererPath;
}

static string* build_debug_page(string* method, string* resource, string* version)
{
	string* s = string_new(200);

	string_concat(s, "Methode: ");
	string_concat_str(s, method);

	string_concat(s, "\nRessource: ");
	string_concat_str(s, resource);

	string_concat(s, "\nVersion: ");
	string_concat_str(s, version);

	return s;
}

//
// Request-Schema
// --------------
// METHOD resource HTTP/VERSION\r\n
// key: value\r\n
// ...\r\n
// \r\n
// Data
//
HttpResponseCodes parse_http_request(char* buffer, size_t bufferSize, HttpRequest* httpRequest, string** staticPage)
{
	if (bufferSize == 0)
		return BAD_REQUEST;

    HttpRequestParsingState parsingState = PARSING_METHOD;
    HttpResponseCodes responseCode = OK;

    string* strMethod = string_new(METHOD_CAPACITY);
    string* strResource = NULL;
    string* strVersion = string_new(VERSION_CAPACITY);
    string* strKey = NULL;
    string* strValue = NULL;

    httpRequest->path = NULL;
    httpRequest->fields = NULL;
    httpRequest->data = NULL;

    debugPage = false;
    methodCode = OK;

    size_t i;

    // iterate through input buffer char by char
    for (i = 0; i < bufferSize && httpRequest->data == NULL && responseCode == OK; i++)
    {
        // current char
        char c = ((char*)(buffer))[i];
        if (c == '\r') // skip backspaces
            continue;

        switch (parsingState)
        {
            case PARSING_METHOD:
                parsingState = parsing_method(c, strMethod, &httpRequest, &responseCode);
                break;

            case PARSING_PATH:
            	parsingState = parsing_path(c, &strResource, &httpRequest, &responseCode);
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

	if (responseCode == OK || debugPage)
	{
		if (debugPage)
		{
			*staticPage = build_debug_page(strMethod, strResource, strVersion);
		}
		else
		{
			string* refererPath = get_referer_path(httpRequest->fields);

			if (refererPath)
            {
		    	// if relative path to image file was delivered by referer-field, insert it at start of strResource
		        if (!string_startswith(strResource, refererPath))
                {
                    string_insert(strResource, refererPath, 0);
                }

	            string_free(refererPath);
            }

		    // validatedPath will only be allocated by validate_path if path is valid (responseCode = OK), and then freed with free_http_request
			string* validatedPath = NULL;
			responseCode = validate_path(strResource, &validatedPath);

			if (responseCode == OK)
			{
				httpRequest->path = validatedPath;
			}
		}
	}

	string_free(strResource);
	string_free(strVersion);
    string_free(strMethod);

    return responseCode;
}
