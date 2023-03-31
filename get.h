// realese

#define MAXBUFLEN 1024

// http get request
int http_get(const char *host, const char *port, const char *path, char* response);

// https get request
int https_get(const char *host, const char *port, const char *path, char *response);

// http get response time
double get_get_response_time(const char* host, int http_or_https);
