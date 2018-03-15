#include <net/net.h>

#include <nstring.h>
#include <string.h>

#ifdef UNIX

#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdio.h>

char *net_get_default_device(void) {
    FILE *f;
    char line[100], *p, *c;
    
    f = fopen("/proc/net/route", "r");
    
    while (fgets(line, 100, f)) {
        p = strtok(line, " \t");
        c = strtok(NULL, " \t");
        
        if (p != NULL && c != NULL) {
            if (strcmp(c, "00000000") == 0) {
                printf("found dev %s\n", p);
                return p;
            }
        }
    }
}

string_t net_get_ip(addr_t *addr) {
    struct in_addr ip_addr = addr->sin_addr;
    string_t ip;
    string_new(&ip);
    char cstr[32];

    inet_ntop(AF_INET, &ip_addr, cstr, IP_STR_LEN);

    string_cat_c(&ip, cstr);
    return ip;
}

string_t net_get_host_ip(const char *adapter) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;

    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_name, adapter, IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    string_t ip;
    string_new(&ip);
    char *inet = inet_ntoa(((addr_t *)&ifr.ifr_addr)->sin_addr);
    printf("INET = %s\n", inet);
    string_cat_c(&ip, inet);

    return ip;
}

#endif