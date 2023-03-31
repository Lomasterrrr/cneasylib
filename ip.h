// realese

// getting frist ip address
char* get_ip_address(const char *host, const char* port, int use_cache);

// gettind all ip addresses
char** get_ip_addresses(const char* host, const char *port, int use_cache);

// debug print frist ip address
int debug_print_ip_address(char *ip, int time);

// debug print all ip addresses
int debug_print_ip_addresses(char **ips, int time);

// to clear the memory after the function: get_ip_addresses
void free_ips(char **ips);
