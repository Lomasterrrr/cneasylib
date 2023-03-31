#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "post.h"
#include "ip.h"


int post(const char *host, int port, char *path, char *content_type, char *request_body, char *response){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Failed to create a socket\n");
        return EOF;
    }

    struct sockaddr_in server_address;

    const char *ip_address = get_ip_address(host, "80", 0);

    server_address.sin_addr.s_addr = inet_addr(ip_address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) != 0) {
        printf("Failed to connect to the server\n");
        return EOF;
    }

    char request[1024];
    sprintf(request, "POST %s HTTP/1.1\r\n", path);
    sprintf(request + strlen(request), "Host: %s:%d\r\n", ip_address, port);
    sprintf(request + strlen(request), "Content-Type: %s\r\n", content_type);
    sprintf(request + strlen(request), "Content-Length: %ld\r\n", strlen(request_body));
    sprintf(request + strlen(request), "\r\n%s", request_body);

    if (send(sock, request, strlen(request), 0) < 0) {
        printf("Failed to send request to server\n");
        return EOF;
    }

    char server_response[BUFFER_SIZE];
    if (recv(sock, server_response, 4096, 0) < 0) {
        printf("Failed to get a response from the server\n");
        return EOF;
    }

    close(sock);

    strcpy(response, server_response);
    return 0;
}

int debug_post(const char *host){
    char response[BUFFER_SIZE];

    if (post(host, 80, "/", "test", "test", response) != EOF){
        printf("%s", response);
        return 0;
    }

    return EOF;
}




