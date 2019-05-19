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

static string* get_absolut_document_root_path()
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

	return absoluteHtdocsDir;
}

static HttpRequestParsingState parsing_method(char c, string** strMethod, HttpRequest** httpRequest, HttpResponseCodes* responseCode)
{
	if (c != ' ')
	{
		if (!(*strMethod))
		{
			*strMethod = string_new(METHOD_CAPACITY);
		}

		// add current char to string
		string_add_char(*strMethod, c);

		// go on with parsing method
		return PARSING_METHOD;
	}
	else
	{
		if (!(*strMethod))
		{
			*responseCode = BAD_REQUEST;
		}
		else
		{
			(*httpRequest)->method = get_method_from_string(*strMethod);
			if ((*httpRequest)->method != GET)
			{
				methodCode = NOT_IMPLEMENTED;
				*responseCode = OK; // liar
			}
		}

		// if current char is a space, start parsing resource
		return PARSING_RESOURCE;
	}
}

static HttpRequestParsingState parsing_resource(char c, string **strPath, HttpRequest **httpRequest, HttpResponseCodes *responseCode)
{
	if (c != ' ')
	{
		if (!(*strPath))
		{
			*strPath = string_new(PATH_CAPACITY);
		}

		// add current char to string
		string_add_char(*strPath, c);

		if ((*strPath)->len > PATH_CAPACITY)
		{
			*responseCode = BAD_REQUEST;
		}

		// go on with parsing resource
		return PARSING_RESOURCE;
	}
	else
	{
		if (!(*strPath))
		{
			*responseCode = BAD_REQUEST;
		}
		else
		{
			url_decode(*strPath);

			if (!string_startswith_cstr(*strPath, "/debug?") && !string_compare_cstr(*strPath, "/debug"))
			{
				*responseCode = methodCode;
			}
			else
			{
				debugPage = true;
			}
		}

		// if current space is a space, start parsing version
		return PARSING_VERSION;
	}
}

static HttpRequestParsingState parsing_version(char c, string** strVersion, HttpRequest** httpRequest, HttpResponseCodes* responseCode)
{
	if (c != '\n')
	{
		if (!(*strVersion))
		{
			*strVersion = string_new(VERSION_CAPACITY);
		}

		// if no new line, add current char to string
		string_add_char(*strVersion, c);

		// go on with parsing version
		return PARSING_VERSION;
	}
	else
	{
		if (!(*strVersion))
		{
			*responseCode = BAD_REQUEST;
		}
		else
		{
			(*httpRequest)->version = validate_version(*strVersion);
			if ((*httpRequest)->version == UNSUPPORTED)
			{
				*responseCode = BAD_REQUEST;
			}
		}

		// start parsing key of next field
		return PARSING_FIELD_KEY;
	}
}

static HttpRequestParsingState parsing_field_key(char c, string** strKey, HttpResponseCodes* responseCode)
{
	if (c == '\n')
	{
		// if current char is new line, start parsing data
		return PARSING_DATA;
	}
	else if (c != ':')
	{
		if (!(*strKey))
		{
			*strKey = string_new(KEY_CAPACITY);
		}

		// add current char to string (ignore case)
		string_add_char(*strKey, tolower(c));

		// go on parsing key of field
		return PARSING_FIELD_KEY;
	}
	else
	{
		if (!(*strKey))
		{
			*responseCode = BAD_REQUEST;
		}

		// if current char is ':', start parsing value of field
		return PARSING_FIELD_VALUE;
	}
}

static HttpRequestParsingState parsing_field_value(char c, string** strKey, string** strValue, HttpRequest** httpRequest, HttpResponseCodes* responseCode)
{
	if (c != '\n')
	{
		if (!(*strValue))
		{
			*strValue = string_new(VALUE_CAPACITY);
		}

		// add current char to string
		string_add_char(*strValue, c);

		// go on parsing field value
		return PARSING_FIELD_VALUE;
	}
	else
	{
		if (!(*strValue))
		{
			*responseCode = BAD_REQUEST;
		}
		else
		{
			string_strip(*strValue);

			Hash pair = SH_create(*strKey, *strValue);
			if (!(*httpRequest)->fields) {
				(*httpRequest)->fields = SHL_create(pair);
			} else {
				SHL_append((*httpRequest)->fields, pair);
			}

			*strKey = NULL;
			*strValue = NULL;
		}

		// if current char is a new line, start parsing key of next field
		return PARSING_FIELD_KEY;
	}
}

// deprecated: was used to make sure that images can be returned if page is called with /index.html (no longer needed with virtual hosting)
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

static HttpResponseCodes get_directory_from_host_field(HashList* fields, string** dir)
{
	HttpResponseCodes responseCode = OK;
	if (fields)
	{
		Hash *hostField = SHL_find_key_cstr(fields, "host");
		if (hostField)
		{
			int splits;
			string** hostFieldParts = string_split(hostField->value, ':', &splits);

			if (string_compare_cstr(hostFieldParts[0], "extern"))
			{
				*dir = string_new_from_cstr("/extern");
			}
			else if (string_compare_cstr(hostFieldParts[0], "intern"))
			{
				responseCode = UNAUTHORIZED;
			}
			else
			{
				*dir = string_new_from_cstr("/default");
			}

			for (int i = 0; i < splits; i++)
				string_free(hostFieldParts[i]);
			free(hostFieldParts);
		}
		else
		{
			*dir = string_new_from_cstr("/default");
		}
	}
	else
	{
		*dir = string_new_from_cstr("/default");
	}

	return responseCode;
}

void free_http_request(HttpRequest* httpRequest)
{
	if (httpRequest->path)
		string_free(httpRequest->path);

	if (httpRequest->fields)
		SHL_remove_all(httpRequest->fields);

	free(httpRequest);
}

HttpResponseCodes validate_resource(HashList* fields, string *resource, string **path)
{
	HttpResponseCodes responseCode;

	string* absolutDocumentRootPath = get_absolut_document_root_path();
	string* subdirectory = NULL;

	responseCode = get_directory_from_host_field(fields, &subdirectory);
	if (responseCode != UNAUTHORIZED)
	{
		string_insert(absolutDocumentRootPath, subdirectory, 0);
		string_free(subdirectory);

		string* tmp = string_copy(absolutDocumentRootPath);
		string_concat_str(tmp, resource);

		// causes 'conditional jump or move depends on uninitialised value' warning
		if (!isfile(tmp))
		{
			string_concat(tmp, "/index.html");
		}

		string_terminate(tmp);

		string* absoluteResourcePath = string_new(PATH_CAPACITY_ABSOLUTE);
		realpath(tmp->buf, absoluteResourcePath->buf);
		absoluteResourcePath->len = strlen(absoluteResourcePath->buf);

		string_free(tmp);

		if (absoluteResourcePath->len < absolutDocumentRootPath->len || !string_startswith(absoluteResourcePath, absolutDocumentRootPath)) // TODO: also check if file access is not allowed (htpasswd)
		{
			string_free(absoluteResourcePath);
			responseCode = FORBIDDEN;
		}
		else
		{
			if (file_exists(absoluteResourcePath))
			{
				*path = absoluteResourcePath;
				responseCode = OK;
			}
			else
			{
				string_free(absoluteResourcePath);
				responseCode = NOT_FOUND;
			}
		}
	}

	string_free(absolutDocumentRootPath);
	return responseCode;
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

    string* strMethod = NULL;
    string* strResource = NULL;
    string* strVersion = NULL;
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
        char c = buffer[i];
        if (c == '\r') // skip backspaces
            continue;

        switch (parsingState)
        {
            case PARSING_METHOD:
                parsingState = parsing_method(c, &strMethod, &httpRequest, &responseCode);
                break;

            case PARSING_RESOURCE:
            	parsingState = parsing_resource(c, &strResource, &httpRequest, &responseCode);
                break;

            case PARSING_VERSION:
            	parsingState = parsing_version(c, &strVersion, &httpRequest, &responseCode);
                break;

            case PARSING_FIELD_KEY:
            	parsingState = parsing_field_key(c, &strKey, &responseCode);
                break;

            case PARSING_FIELD_VALUE:
            	parsingState = parsing_field_value(c, &strKey, &strValue, &httpRequest, &responseCode);
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
			responseCode = OK;
			*staticPage = build_debug_page(strMethod, strResource, strVersion);
		}
		else
		{
			// deprecated: was used to make sure that images can be returned if page is called with /index.html (no longer needed with virtual hosting)
//			string* refererPath = get_referer_path(httpRequest->fields);
//			if (refererPath)
//			{
//				// if relative path to image file was delivered by referer-field, insert it at start of strResource
//				if (!string_startswith(strResource, refererPath))
//				{
//					string_insert(strResource, refererPath, 0);
//				}
//
//				string_free(refererPath);
//			}

			// validatedPath will only be allocated by validate_resource if path is valid (responseCode = OK), and then freed with free_http_request
			string* validatedPath = NULL;
			responseCode = validate_resource(httpRequest->fields, strResource, &validatedPath);

			if (responseCode == OK)
			{
				httpRequest->path = validatedPath;
			}
		}
	}

	string_free(strValue);
	string_free(strKey);
	string_free(strResource);
	string_free(strVersion);
    string_free(strMethod);

    return responseCode;
}
