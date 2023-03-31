#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <time.h>
#include "get.h"

int http_get(const char *host, const char *port, const char *path, char *response) {
    struct addrinfo hints, *server_info;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(host, port, &hints, &server_info);

    int sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    connect(sockfd, server_info->ai_addr, server_info->ai_addrlen);

    char request[MAXBUFLEN];
    sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);
    send(sockfd, request, strlen(request), 0);

    int numbytes = recv(sockfd, response, MAXBUFLEN, 0);
    if (numbytes == -1) {
        perror("recv");
        return EOF;
    }

    response[numbytes + 1] = '\0';

    freeaddrinfo(server_info);
    close(sockfd);

    return 0;
}

int https_get(const char *host, const char *port, const char *path, char *response){
    int sockfd, numbytes;
    struct addrinfo hints, *servinfo, *p;
    char buf[MAXBUFLEN];
    SSL_CTX *ctx;
    SSL *ssl;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(host, port, &hints, &servinfo) != 0) {
        fprintf(stderr, "getaddrinfo error\n");
        return EOF;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to connect to server\n");
        return EOF;
    }

    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL) {
        fprintf(stderr, "Failed to create SSL context\n");
        return EOF;
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);

    if (SSL_connect(ssl) == -1) {
        ERR_print_errors_fp(stderr);
        return EOF;
    }

    snprintf(buf, MAXBUFLEN, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);

    if (SSL_write(ssl, buf, strlen(buf)) == -1) {
        ERR_print_errors_fp(stderr);
        return EOF;
    }

    numbytes = SSL_read(ssl, buf, MAXBUFLEN-1);
    if (numbytes == -1) {
        ERR_print_errors_fp(stderr);
        return EOF;
    }
    buf[numbytes] = '\0';
    strcpy(response, buf);

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sockfd);

    return 0;
}

double get_get_response_time(const char *host, int http_or_https){
    clock_t start,end; 
    double ms;

    start = clock();
    char temp[MAXBUFLEN];
    int status;

    if (http_or_https == 0){
        status = http_get(host,"80","/",temp);
    }
    else if (http_or_https == 1){
        status = https_get(host,"443","/",temp);
    }
    else {
        return EOF;
    }

    if (status != EOF){
        end = clock();
        ms = (double) (end - start) * 1000 / CLOCKS_PER_SEC;
        return ms;
    }

    return EOF;
}


