// realese

#define BUFFER_SIZE 4096

// send post request
int post(const char* host, int port, char* path, char* content_type, char* request_body, char* response);
int debug_post(const char* host);
