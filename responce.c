#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unistd.h>

int get_responce_code(const char *host, int port, const char* path){
    SSL_CTX *ctx;
    SSL *ssl;
    int sockfd, status, len, http_status_code;
    char buffer[1024];
    char request[1024];
    const char *_host = host;
    struct hostent *server;
    struct sockaddr_in serv_addr;

    if (strncmp(host, "https://", 8) == 0) {
        _host += 8;
        ctx = SSL_CTX_new(TLS_client_method());
    } else {
        ctx = NULL;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("socket error");
        return EOF;
    }

    server = gethostbyname(_host);

    if (server == NULL) {
        fprintf(stderr, "gethostbyname error: %s\n", hstrerror(h_errno));
        return EOF;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if (strncmp(host, "https://", 8) == 0) {
        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (sockfd < 0) {
            perror("socket error");
            return EOF;
        }

        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("connect error");
            return EOF;
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sockfd);
        SSL_connect(ssl);

        len = snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, _host);
        SSL_write(ssl, request, len);

        while ((len = SSL_read(ssl, buffer, sizeof(buffer))) > 0) {
            if (strncmp(buffer, "HTTP/1.", 7) == 0) {
                http_status_code = atoi(&buffer[9]);
                break;
            }
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
    } else {
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("connect error");
            return EOF;
        }

        len = snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, _host);
        send(sockfd, request, len, 0);

        while ((len = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
            if (strncmp(buffer, "HTTP/1.", 7) == 0) {
                http_status_code = atoi(&buffer[9]);
                break;
            }
        }
    }

    close(sockfd);
    if (ctx != NULL) {
        SSL_CTX_free(ctx);
    }

    return http_status_code;
}
