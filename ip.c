#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ip.h"
#include <time.h>

#define CACHE_SIZE_IP1 10
#define CACHE_SIZE_IP2 10

struct cache_entry_ip1 {
    char host[256];
    char port[6];
    char ip[INET6_ADDRSTRLEN];
};

struct cache_entry_ip2 {
    char host[256];
    char port[6];
    char ip[INET6_ADDRSTRLEN];
};

struct cache_entry_ip1 cache[CACHE_SIZE_IP1] = {0};
struct cache_entry_ip2 cache1[CACHE_SIZE_IP2] = {0};

char* get_ip_address(const char *host, const char *port, int use_cache) {
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if (use_cache) {
        for (int i = 0; i < CACHE_SIZE_IP1; i++) {
            if (strcmp(cache[i].host, host) == 0 && strcmp(cache[i].port, port) == 0) {
                return cache[i].ip;
            }
        }
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(host, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return NULL;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        void *addr;

        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        }
        else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        strncpy(cache[CACHE_SIZE_IP1-1].host, host, sizeof cache[CACHE_SIZE_IP1-1].host);
        strncpy(cache[CACHE_SIZE_IP1-1].port, port, sizeof cache[CACHE_SIZE_IP1-1].port);
        strncpy(cache[CACHE_SIZE_IP1-1].ip, ipstr, sizeof cache[CACHE_SIZE_IP1-1].ip);

        for (int i = CACHE_SIZE_IP1 - 1; i > 0; i--) {
            memcpy(&cache[i-1], &cache[i], sizeof(cache[i]));
        }

        break;
    }

    freeaddrinfo(res);

    if (p == NULL) {
        fprintf(stderr, "Unable to connect to %s\n", host);
        return NULL;
    }

    return cache[CACHE_SIZE_IP1 - 1].ip;
}

char **get_ip_addresses(const char *host, const char *port, int use_cache){
    struct addrinfo hints, *res, *p;
    int status;
    char **ips = NULL;
    char ipstr[INET6_ADDRSTRLEN];
    int i = 0;

    if (use_cache) {
        for (i = 0; i < CACHE_SIZE_IP2; i++) {
            if (strcmp(cache1[i].host, host) == 0 && strcmp(cache1[i].port, port) == 0) {
                break;
            }
        }

        if (i < CACHE_SIZE_IP2 && cache1[i].ip[0] != '\0') {
            char **ips = malloc((i+2) * sizeof(char *));
            for (int j = 0; j <= i; j++) {
                ips[j] = strdup(cache1[j].ip);
            }
            ips[i+1] = NULL;
            return ips;
        }
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(host, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return NULL;
    }

    int count = 0;
    for (p = res; p != NULL; p = p->ai_next) {
        count++;
    }

    ips = malloc((count+1) * sizeof(char *));
    ips[count] = NULL;

    for (i = 0, p = res; p != NULL; i++, p = p->ai_next) {
        void *addr;

        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } 
        else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        strncpy(cache1[CACHE_SIZE_IP2-1].host, host, sizeof cache1[CACHE_SIZE_IP2-1].host);
        strncpy(cache1[CACHE_SIZE_IP2-1].port, port, sizeof cache1[CACHE_SIZE_IP2-1].port); 
        strncpy(cache1[CACHE_SIZE_IP2-1].ip, ipstr, sizeof cache1[CACHE_SIZE_IP2-1].ip);

        for (int j = CACHE_SIZE_IP2-1; j > 0; j--) {
            memcpy(&cache1[j-1], &cache1[j], sizeof(struct cache_entry_ip2));
        }

        ips[i] = strdup(ipstr);
    }
    freeaddrinfo(res);
    return ips;
}

void free_ips(char **ips){
    char **p;
    for (p = ips; *p != NULL; p++) {
        free(*p);
    }
    free(ips);
}

int debug_print_ip_addresses(char **ips, int time){
    clock_t start,end; 
    double ms;
    char **temp;

    if (time){
        start = clock();
    }

    if (ips == NULL){
        fprintf(stderr, "error: getting IP addresses");
        return EOF;
    }

    for (temp = ips; *temp != NULL; temp++){
        printf("%s\n", *temp);
    }

    free_ips(ips);

    if (time){
        end = clock();
        ms = (double) (end - start) * 1000 / CLOCKS_PER_SEC;
        printf("Program executed in %f milliseconds.\n", ms);
    }

    return 0;
}

int debug_print_ip_address(char *ip, int time){
    clock_t start,end; 
    double ms;
    
    if (time){
        start = clock();
    }

    if (ip){
        printf("%s\n", ip);
    }
    else {
        fprintf(stderr, "error: getting IP address");
        return EOF;
    }

    if (time){
        end = clock();
        ms = (double) (end - start) * 1000 / CLOCKS_PER_SEC;
        printf("Program executed in %f milliseconds.\n", ms);
    }

    return 0;
}



