/* Version: 2.1.1 */
#include <errno.h>      // errno
#include <netinet/ip.h> // ^
#include <signal.h>     // sigaction, struct sigaction, siginfo_t, SIGTERM
#include <stdbool.h>    // bool
#include <stdio.h>      // fprintf
#include <stdlib.h>     // exit, malloc, free
#include <string.h>     // memset, strerror
#include <sys/socket.h> // struct sockaddr_in, socket, setsockopt, bind, listen, socklen_t, accept
#include <unistd.h> // read, write, close
#include <poll.h>
#include "request.h"
#include "response.h"

#define PORT 31337
#define BUFFER_SIZE 1024
#define MAX_HEADER_LEN 8192 // because apache allows a max header length of. 8 KB

/**
 * Globale Variablen.
 */
static bool run = true;

/**
 * Fehlerbehandlung.
 */
static void error(char *msg) {
    fprintf(stderr, "%s", msg);
    if (errno) {
        fprintf(stderr, ", errno: %s", strerror(errno));
    }
    fprintf(stderr, "\n");
    exit(1);
}

/**
 * Behandlung des SIGINT-Signals (Strg+C) um den Server zu beenden.
 */
static void handle_signal(int signum) {
    if (signum != SIGINT) {
        error("ERROR unexpected signal");
    }

    /*
     * Beende den Server nach dem Abarbeiten des letzten Clients.
     */
    run = false;
}

/**
 * Registriert das SIGINT-Signal (Strg+C) um den Server beenden zu können.
 */
static void register_signal() {
    struct sigaction action;

    /*
     * Konfigurieren des Signal-Handlers.
     */
    memset(&action, 0, sizeof(action));
    action.sa_handler = handle_signal;

    /*
     * Registrierung des Signal-Handlers.
     */
    if (sigaction(SIGINT, &action, NULL) < 0) {
        error("ERROR registering signal handler");
    }
}

/**
 * Erstellt und konfiguriert den Netzwerk-Socket, über den die Verbindungen
 * angenommen werden.
 */
static int setup_socket() {
#ifdef STDIN_ONLY
    return STDOUT_FILENO;
#endif

    int opt = 1;
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    /*
     * Setzt Konfigurationsvariablen für den Socket, z.B. die Portnummer.
     */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    /*
     * Erstelle den Socket.
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    /*
     * Verwende den Socket, selbst wenn er aus einer vorigen Ausführung
     * im TIME_WAIT Status ist.
     */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt,
                   sizeof(int)) < 0)
        error("ERROR on setsockopt,SO_REUSEADDR");

    /*
     * Melde, dass der Socket eingehende Verbindungen akzeptieren soll.
     */
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    /*
     * Horche auf dem Socket nach eingehenden Verbindungen. Es werden maximal
     * fünf gleichzeitige Verbindungen erlaubt.
     */
    if (listen(sockfd, 5) < 0) {
        error("listen");
    }
    return sockfd;
}

/**
 * Die Hauptschleife, in der eingehende Verbindungen angenommen werden.
 */
static void main_loop(int sockfd) {
    int newsockfd;
    ssize_t length;

#ifndef STDIN_ONLY
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
#endif

    void* const buffer = malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        error("ERROR at malloc.");
    }

    /*
     * Die Hauptshleife des Programms.
     */
    while (run) {

#ifndef STDIN_ONLY
        /*
         * Der accept()-Aufruf blockiert, bis eine neue Verbindung rein kommt.
         */
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            /*
             * Wenn der Server mit dem SIGINT-Signal beendet wird, schlägt accept
             * mit EINTR (interrupted) fehl.
             */
            if (errno == EINTR) {
                break;
            }
            error("ERROR on accept");
        }
#else
        newsockfd = STDIN_FILENO;
#endif

	    /*
	    * Dafür sorgen, dass die Verbindung nach einer bestimmten Zeit geschlossen wird
	    */
	    struct timeval tv;
	    tv.tv_sec = 1;
	    tv.tv_usec = 0;
	    if (setsockopt(newsockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0)
		    error("ERROR on setsockopt, SO_RCVTIMEO");

        string* strRequest = string_new(MAX_HEADER_LEN);

        bool timeouted = false;
        length = 0;

        // Fill strRequest until \r\n\r\n is found
	    do
	    {
	    	if (strRequest->len > MAX_HEADER_LEN)
		    {
	    		timeouted = true;
		    }
	    	else
	    	{
			    /*
                 * Lies die ankommenden Daten von dem Socket in das Array buffer.
                 */
			    memset(buffer, 0, BUFFER_SIZE);
			    length = read(newsockfd, buffer, BUFFER_SIZE);
			    if (length < 0)
			    {
				    timeouted = true;
				    break;
			    }
	    	}
	    }
	    while (fill_request_string(strRequest, buffer, length));

	    // If timeouted, close connection to client (apache like)
	    if (timeouted)
	    {
		    if (close(newsockfd) < 0)
			    error("ERROR on close");

		    string_free(strRequest);
		    continue;
	    }

	    string* staticPage = NULL;
	    HttpRequest* httpRequest = NULL;
	    HttpResponseCode responseCode = parse_http_request(strRequest, &httpRequest, &staticPage);

	    string_free(strRequest);



//        string_free(strRequest);

//        string* staticPage = NULL;
//        HttpRequest* httpRequest = calloc(sizeof(HttpRequest), 1);
//        HttpResponseCode responseCode = parse_http_request(buffer, length, httpRequest, &staticPage);

/*
 * Schreibe die ausgehenden Daten auf den Socket.
 */
#ifndef STDIN_ONLY
//	    send_http_response(newsockfd, responseCode, httpRequest->path, staticPage);
        length = write(newsockfd, buffer, (size_t)length);
        if (length < 0) {
            error("ERROR writing to socket");
        }
#else
	    send_http_response(STDOUT_FILENO, responseCode, httpRequest->path, staticPage);
//        /*
//     * Gib die eingegangenen Daten auf der Kommandozeile aus.
//     */
//    if (write(STDOUT_FILENO, buffer, length) < 0) {
//      error("ERROR writing to STDOUT");
//    }
#endif

//		string_free(staticPage);
//	    free_http_request(httpRequest);

/*
 * Schließe die Verbindung.
 */
#ifndef STDIN_ONLY
        if (close(newsockfd) < 0) {
            error("ERROR on close");
        }
#endif
    }

    /*
     * Lösche den Buffer und schließe den Socket. Dieser Aufruf sollte wegen der
     * Endlosschleife niemals ausgeführt werden.
     */
    free(buffer);
#ifndef STDIN_ONLY
    if (close(sockfd) < 0) {
        error("ERROR on close");
    }
#endif
}


void test_hashlist(){

    string* s1 = string_new_from_cstr("Pepsi ist besser als");
    string* s2 = string_new_from_cstr("coca cola");
    string* s3 = string_new_from_cstr("Hummeln sind");
    string* s4 = string_new_from_cstr("Bienen");
    string* s5 = string_new_from_cstr("Henning fand es");
    string* s6 = string_new_from_cstr("lustig");
    string* s7 = string_new_from_cstr("Ctype from Destop/LMAO: ");
    string* s8 = get_content_type("/home/bjoern/Desktop/LMAO");

    HashList* list = SHL_create(SH_create(s1,s2));
    SHL_append(list, SH_create(s3, s4));
    SHL_append(list, SH_create(s5, s6));
    SHL_append(list, SH_create(s7, s8));

    int i = SHL_get_size(list);

    printf("%d", i);

    for (int j = 0; j < i; ++j) {
        string_print(SHL_at(list, j).key);
        string_print(SHL_at(list, j).value);
    }

    SHL_remove_all(list);
}

//this seems to be buggy, but im too tired to fix it now
void test_response_build(){

    string* s1 = string_new(15);
    string* s2 = string_new(15);
    string* s3 = string_new(15);
    string* s4 = string_new(15);

    string_concat(s1, "Name");
    string_concat(s2, "surwer");
    string_concat(s3, "yabba");
    string_concat(s4, "dabbadooh");

    HashList* list = SHL_create(SH_create(s1,s2));
    SHL_append(list, SH_create(s3,s4));

    string *s = build_http_response_header(OK, list);

    string_print(s);

    string_free(s);
    SHL_remove_all(list);
}

void test_string_split(){
    string* str = string_new(10);
    string_concat(str, "SPLITBY:");

    int splits;
    string** splitted = string_split(str, ':', &splits);

    printf("%d", splits);

    for (int i = 0; i < splits; ++i) {
        printf("String %d:", i);
        string_print(splitted[i]);
    }

    string_free(str);
    free(splitted);
}

#include "helpers.h"

void test_get_ctype() {
    string* type = get_content_type("/home/bjoern/Desktop/LMAO");

    if(type) {
        string_print(type);
        string_free(type);
    }
}

void test_string_insert(){
    string* dst = string_new_from_cstr("Ha Welt");
    string* src = string_new_from_cstr("lo");

    string_insert_cstr(src, "l", 1);
    string_insert(dst, src, 2);

    string_print(dst);

    string_free(dst);
    string_free(src);
}

void test_string_split_string(){
    string* str = string_new_from_cstr("einsLLzweiLLdreiLL");


    int splits;

    string** arr = string_split_cstr(str, "LL", &splits);

    printf("%d\n", splits);

    for (int i = 0; i < splits; ++i) {
        string_print(arr[i]);
        string_free(arr[i]);
    }

    string_free(str);
    free(arr);
}

int main(int argc, char *argv[]) {
//    test_response_build();    //works
//    test_hashlist();          //works
//    test_string_split();      //works again
//    test_get_ctype();
//    test_string_insert();
//    test_string_split_string();

    (void)argc;
    (void)argv;
    register_signal();
    const int sockfd = setup_socket();
    main_loop(sockfd);

    return 0;
}
