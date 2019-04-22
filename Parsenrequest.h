//
// Created by Leon on 21.04.2019.
//
//Following code is used from https://codereview.stackexchange.com/questions/188384/http-request-parser-in-c
#ifndef PSE_PARSENREQUEST_H
#define PSE_PARSENREQUEST_H
//Typedef of the Method
typedef enum Method {
    UNSUPPORTED, GET, HEAD
} Method;
//A struct for the Header

typedef struct Header {
    char *name;
    char *value;
    struct Header *next;
} Header;

//Typedef of the Request
typedef struct Request {
    enum Method met;
    char *url;
    char *version;
    struct Header *headers;
    char *body;
} Request;

//The request gets parsed and returns it as a struct Request.
struct Request *parse_request(const char *raw);

void free_header(struct Header *h);

void free_request(struct Request *req);

#endif //PSE_PARSENREQUEST_H
