#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"


char *compute_delete_request(char *host, char *url, char* query_params,
                            char *cookies, int cookies_count, char *body_data) {

    char *message = (char*) calloc(BUFLEN, sizeof(char));
    char *line = (char*) calloc(LINELEN, sizeof(char));
   // Writes the method name, URL and protocol type
   if (query_params != NULL) {
        sprintf(line, "DELETE %s%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Adds the host
	sprintf(line, "HOST: %s", host);
	compute_message(message, line);
    char null_char[BUFLEN];
    memset(null_char, 0, BUFLEN);
    // Adds cookies
    if (cookies_count && strlen(cookies)) {
       sprintf(line, "Cookie: %s;", cookies);
	    compute_message(message, line);
    }

    if (body_data != NULL && strlen(body_data)) {
        sprintf(line, "Authorization: Bearer %s", body_data);
	    compute_message(message, line);
    }
    // Adds final new line
    compute_message(message, "");

    return message;
}

char *compute_get_request(char *host, char *url, char* query_params,
                            char *cookies, int cookies_count, char *body_data) {

    char *message = (char*) calloc(BUFLEN, sizeof(char));
    char *line = (char*) calloc(LINELEN, sizeof(char));

    // Writes the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Adds the host
	sprintf(line, "HOST: %s", host);
	compute_message(message, line);

    // Adds headers and/or cookies, according to the protocol format
    if(cookies != NULL) {
        if (strlen(cookies)) {
        sprintf(line, "Cookie: %s;", cookies);
            compute_message(message, line);
        }
    }
    
    if (body_data != NULL) {
        if(strlen(body_data)) {
            sprintf(line, "Authorization: Bearer %s", body_data);
	        compute_message(message, line);
        }
    }

    // Adds final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
                            int body_data_fields_count, char *cookies, int cookies_count, char *data) {

    char *message =(char*) calloc(BUFLEN, sizeof(char));
    char *line = (char*) calloc(LINELEN, sizeof(char));

    // Writes the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Adds the host.
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);

    /* Adds necessary headers (Content-Type and Content-Length are mandatory)
        in order to write Content-Length you must first compute the message size
    */
    compute_message(message, "Content-Type: application/json");
    
    sprintf(line,"Content-Length: %d", (int) strlen(body_data));
    compute_message(message,line);
   
    // Adds cookies
    if (cookies != NULL) {
        if (cookies_count && strlen(cookies)) {
        sprintf(line, "Cookie: %s;", cookies);
            compute_message(message, line);
        }
    }
    if (data != NULL) {
        if (strlen(data)) {
            sprintf(line, "Authorization: Bearer %s", data);
            compute_message(message, line);
        }
    }
    
    // Adds new line at end of header
    compute_message(message,"");
    // Adds the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data);

    free(line);
    return message;
}
