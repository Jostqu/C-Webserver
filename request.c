//
// Created by Marcel on 26.04.2019.
//

#include "request.h"

#define PATH_CAPACITY 2083
#define PATH_CAPACITY_ABSOLUTE (PATH_CAPACITY + 200)

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

static string* build_debug_page(string* method, string* resource, string* version)
{
	string* s = string_new(200);

	string_concat(s, "Method: ");
	string_concat_str(s, method);

	string_concat(s, "\nResource: ");
	string_concat_str(s, resource);

	string_concat(s, "\nVersion: ");
	string_concat_str(s, version);

	return s;
}

static HttpResponseCode get_directory_from_host_field(HashList* fields, string** dir)
{
	HttpResponseCode responseCode = OK;
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
			responseCode = BAD_REQUEST;
		}
	}
	else
	{
		responseCode = BAD_REQUEST;
	}

	return responseCode;
}

static HttpResponseCode parse_first_line(string* firstHeaderLine, HttpRequest* httpRequest, string** debugPage)
{
	HttpResponseCode responseCode = OK;

	// parse first line with method, resource and version
	int firstLineSplits;
	string** firstLine = string_split(firstHeaderLine, ' ', &firstLineSplits);
	if (firstLineSplits != 3)
	{
		responseCode = BAD_REQUEST;
	}
	else
	{
		// build the debug page, for /debug
		*debugPage = build_debug_page(firstLine[0], firstLine[1], firstLine[2]);

		httpRequest->method = get_method_from_string(firstLine[0]);

		httpRequest->resource = string_copy(firstLine[1]); // is validated later because it depends on some fields
		url_decode(httpRequest->resource);

		httpRequest->version = validate_version(firstLine[2]);

		if (httpRequest->method == INVALID)
		{
			responseCode = NOT_IMPLEMENTED;
		}
		else if (httpRequest->method != GET)
		{
			responseCode = METHOD_NOT_ALLOWED;
		}
		else
		{
			if (httpRequest->version == UNSUPPORTED)
			{
				responseCode = VERSION_NOT_SUPPORTED;
			}
		}
	}

	string_free_stringlist(firstLine, firstLineSplits);

	return responseCode;
}

static HttpResponseCode parse_fields(string** headerLines, int headerLineCount, HttpRequest* httpRequest)
{
	HttpResponseCode responseCode = OK;
	for (int i = 1; i < headerLineCount; i++)
	{
		int fieldSplits;
		string** field = string_split(headerLines[i], ':', &fieldSplits);
		if (fieldSplits < 2)
		{
			responseCode = BAD_REQUEST;
		}
		else
		{
			string* key = field[0];
			string_to_lower(key);

			bool keyIsValid = true;
			for (int x = 0; x < key->len && keyIsValid; x++)
			{
				char c = key->buf[x];
				if (!isalpha(c) && !isdigit(c) && c != '-')
					keyIsValid = false;
			}

			if (!keyIsValid)
			{
				responseCode = BAD_REQUEST;
			}
			else
			{
				string* value = string_strip(string_join(field + 1, fieldSplits - 1, ':'));

				Hash fieldhash = SH_create(string_copy(key), value);
				if (!httpRequest->fields)
				{
					httpRequest->fields = SHL_create(fieldhash);
				}
				else
				{
					SHL_append(httpRequest->fields, fieldhash);
				}
			}
		}

		string_free_stringlist(field, fieldSplits);
	}

	return responseCode;
}


void free_http_request(HttpRequest* httpRequest)
{
	if (httpRequest)
	{
		if (httpRequest->resource)
			string_free(httpRequest->resource);

		if (httpRequest->fields)
			SHL_remove_all(httpRequest->fields);

		free(httpRequest);
	}
}

HttpResponseCode validate_resource(HashList* fields, string *resource, string **path)
{
	HttpResponseCode responseCode;

	string* absolutDocumentRootPath = get_absolut_document_root_path();
	string* subdirectory = NULL;

	responseCode = get_directory_from_host_field(fields, &subdirectory);
	if (responseCode == OK)
	{
		string_concat_str(absolutDocumentRootPath, subdirectory);
		string_free(subdirectory);

		string* tmp = string_copy(absolutDocumentRootPath);
		string_concat_str(tmp, resource);

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
HttpResponseCode parse_http_request(string* request, HttpRequest** httpRequest, string** staticPage)
{
	HttpResponseCode responseCode = OK;

	int requestPartCount;
	string** requestParts = string_split_cstr(request, "\r\n\r\n", &requestPartCount);

	string* header = requestParts[0];
	string* debugPage = NULL;

	*httpRequest = calloc(1, sizeof(HttpRequest));

	int headerLineCount;
	string** headerLines = string_split_cstr(header, "\r\n", &headerLineCount);
	if (headerLineCount < 2) // we need at least the first line with method, resource and version, and the host field
	{
		responseCode = BAD_REQUEST;
	}
	else
	{
		responseCode = parse_first_line(headerLines[0], *httpRequest, &debugPage);

		if (responseCode == OK)
		{
			responseCode = parse_fields(headerLines, headerLineCount, *httpRequest);
		}
	}

	string_free_stringlist(headerLines, headerLineCount);
	string_free_stringlist(requestParts, requestPartCount);

	if ((*httpRequest)->resource && (string_compare_cstr((*httpRequest)->resource, "/debug") || string_startswith_cstr((*httpRequest)->resource, "/debug?")))
	{
		*staticPage = debugPage;
		responseCode = OK;
	}
	else
	{
		if (responseCode == OK)
		{
			// validatedPath will only be allocated by validate_resource if path is valid (responseCode = OK), and then freed with free_http_request
			string* validatedPath = NULL;
			responseCode = validate_resource((*httpRequest)->fields, (*httpRequest)->resource, &validatedPath);

			if (responseCode == OK)
			{
				string_free((*httpRequest)->resource);
				(*httpRequest)->resource = validatedPath;
			}
		}

		string_free(debugPage);
	}

	printf("responseCode: %s\n", code_to_string(responseCode));
	return responseCode;
}

bool fill_request_string(string *strRequest, char *buffer, size_t bufferSize)
{
	// append buffer to the request string
	string* strBuffer = string_new_from_carray(buffer, bufferSize);
	string_concat_str(strRequest, strBuffer);
	string_free(strBuffer);

	// try to find '\r\n\r\n' (separator between header and data)
	int splits;
	string** strBufferParts = string_split_cstr(strRequest, "\r\n\r\n", &splits);

	for (int i = 0; i < splits; i++)
		string_free(strBufferParts[i]);
	free(strBufferParts);

	return splits < 2; // we need at least two splits (header and data)
}
