#define MAX_PORTS 65535
#define TIMEOUT 800 // only debug_processing_ports (ms)

/* 
scan one port from one ip
return value:
   -3 = socket error
   -2 = ip not valid
   -1 = other error
    0 = open
    1 = closed
    2 = filtered 
*/
int scan_port(const char *ip, int port, int timeout_ms);

// print start menu and print processing results
void debug_processing_ports(const char *ip, int *ports, int num_ports);

// test tpc scan ip array and port array
void debug_scan_ports(int *ports, int num_ports, char **ips, int num_ips);

