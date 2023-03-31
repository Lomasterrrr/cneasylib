#include <asm-generic/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <wchar.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "tpcscan.h"

int scan_port(const char *ip, int port, int timeout_ms) {
    int sock;
    struct sockaddr_in target;
    struct timeval timeout;
    fd_set fds;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket() failed");
        return EOF;
    }

    target.sin_family = AF_INET;
    target.sin_addr.s_addr = inet_addr(ip);
    target.sin_port = htons(port);

    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    int ret = connect(sock, (struct sockaddr *)&target, sizeof(target));

    if (ret == 0) {
        close(sock);
        return 0;
    }

    if (ret < 0 && errno != EINPROGRESS) {
        if (errno == ECONNREFUSED) {
            close(sock);
            return 1;
        } 
        else {
            close(sock);
            return 2;
        }
    }

    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    ret = select(sock + 1, NULL, &fds, NULL, &timeout);

    if (ret == 0) {
        close(sock);
        return 1;
    }

    if (ret < 0) {
        if (errno == EAGAIN) {
            close(sock);
            return 2;
        } 
        else {
            perror("select() failed");
            close(sock);
            return EOF;
        }
    }

    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len);

    if (err == 0 && FD_ISSET(sock, &fds)) {
        close(sock);
        return 0;
    }

    close(sock);
    return EOF;
}

void debug_processing_ports(const char *ip, int *ports, int num_ports) {
    for (int i = 0; i < num_ports; i++) {
        if (scan_port(ip, ports[i], TIMEOUT) == 0){
            printf("Port %d is open on %s\n", ports[i], ip);
        }
        else if (scan_port(ip, ports[i], TIMEOUT) == 2){
            printf("Port %d is filtered on %s\n", ports[i], ip);
        }
        else if (scan_port(ip, ports[i], TIMEOUT) == 1){
            printf("Port %d is closed on %s\n", ports[i], ip);
        }
        else{
            printf("Port %d is ERROR on %s\n", ports[i], ip);
        }
    }
}

void debug_scan_ports(int *ports, int num_ports, char **ips, int num_ips) {
    for (int i = 0; i < num_ips; i++) {
        debug_processing_ports(ips[i], ports, num_ports);
    }
}
