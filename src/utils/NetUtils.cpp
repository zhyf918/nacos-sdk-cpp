#include "NetUtils.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <Debug.h>
#include <string.h>

namespace nacos{

NacosString NetUtils::getHostIp() throw(NacosException){

    struct ifaddrs *ifaddr, *ifa;
    int s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1)
    {
        throw new NacosException(NacosException::UNABLE_TO_GET_HOST_IP, "Failed to get IF address");
    }


    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        log_debug("iterating on iface=%s\n", ifa->ifa_name);
        if (ifa->ifa_addr == NULL || !(ifa->ifa_addr->sa_family==AF_INET)) {
            continue;
        }

        if((strcmp(ifa->ifa_name,"lo")==0)) {
            continue;
        }

        s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (s != 0) {
            freeifaddrs(ifaddr);
            throw new NacosException(NacosException::UNABLE_TO_GET_HOST_IP, "Failed to get IF address");
        }

        log_debug("selected iface=%s ip=%s\n", ifa->ifa_name, host);
        freeifaddrs(ifaddr);
        return host;
    }
    //Usually the program will not run to here
    throw new NacosException(NacosException::UNABLE_TO_GET_HOST_IP, "Failed to get IF address");
}
}//namespace nacos
