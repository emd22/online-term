#include <net/net.h>

#include <string.h>

#ifdef UNIX

#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdio.h>

char *net_get_default_device(char *dev) {
    FILE *f;
    char line[128], *p, *c;
    
    f = fopen("/proc/net/route", "r");
    
    while (fgets(line, 128, f)) {
        p = strtok(line, " \t");
        c = strtok(NULL, " \t");
        
        if (p != NULL && c != NULL) {
            if (strcmp(c, "00000000") == 0) {
                // printf("found dev %s\n", p);
                // we found the device we need!
                strcpy(dev, p);
                return dev;
            }
        }
    }
    return NULL;
}

char *net_get_ip(struct sockaddr_in *addr, char *ipstr) {
    struct in_addr ip_addr = addr->sin_addr;
    memset(ipstr, 0, 32);

    inet_ntop(AF_INET, &ip_addr, ipstr, 32);

    return ipstr;
}

char *net_get_host_ip(char *adapter) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;

    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_name, adapter, IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);
    
    char *inet = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    printf("INET: %s\n", inet);
    return inet;
}

#endif