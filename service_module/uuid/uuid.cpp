#include "uuid.h"
#ifdef WIN32
#include <WinSock2.h>
#include <WinBase.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

namespace Elephants
{
boost::mutex UUID::mtx;
int UUID::counter = 1;

UUID::UUID()
{
    generator();
}

UUID::UUID(const UUID&  id): value(id.value)
{

}

UUID UUID::operator=(const UUID& id)
{
    value = id.value;
    return *this;
}

bool UUID::operator<(const UUID& id)
{
    return value < id.value;
}

bool UUID::operator==(const UUID& id)
{
    return value == id.value;
}

bool UUID::getIP(IPV4& ip)
{
    char szBuffer[1024];

#ifdef WIN32
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 0);
    if(::WSAStartup(wVersionRequested, &wsaData) != 0)
        return false;
#endif


    if(gethostname(szBuffer, sizeof(szBuffer)) == -1)
    {
#ifdef WIN32
        WSACleanup();
#endif
        return false;
    }

    struct hostent *host = gethostbyname(szBuffer);
    if(host == NULL)
    {
#ifdef WIN32
        WSACleanup();
#endif
        return false;
    }

#ifdef WIN32
    //Obtain the computer's IP
    ip.b1 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b1;
    ip.b2 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b2;
    ip.b3 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b3;
    ip.b4 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b4;
#else
    for (int i = 0; host->h_addr_list[i] != NULL; ++i)
    {
        if (host->h_addrtype == AF_INET)
        {
            struct in_addr* paddr = (struct in_addr*)(host->h_addr_list[i]);
            ip.b1 = 0xff & paddr->s_addr;
            ip.b2 = 0xff & (paddr->s_addr >> 8);
            ip.b3 = 0xff & (paddr->s_addr >> 16);
            ip.b4 = 0xff & (paddr->s_addr >> 24);
        }
        else
        {
            struct in6_addr* paddr = (struct in6_addr*)(host->h_addr_list[i]);
            if (in6addr_loopback == *paddr)
            {
                continue;
            }
            ip.b1 = paddr->s6_addr[0];
            ip.b2 = paddr->s6_addr[1];
            ip.b3 = paddr->s6_addr[2];
            ip.b4 = paddr->s6_addr[3];
        }
    }
#endif

#ifdef WIN32
    WSACleanup();
#endif
    return true;
}


std::string UUID::get_id()
{
    return value;
}

void UUID::generator()
{
    // the seconds since the Unix epoch
    char tstr[16] = { 0 };
    sprintf(tstr, "%08x", time(NULL));
    // machine id
    char ip[16] = { 0 };
    IPV4  res;
    if (getIP(res))
    {
        sprintf(ip, "%02x%02x%02x%02x", res.b1,res.b2,res.b3,res.b4);
    }
    else
    {
        srand((unsigned)(time(NULL)));
        std::size_t rd = std::rand();
        sprintf(ip,"%08x", rd);
    }
    // process id
#ifdef WIN32
    unsigned long pid = GetCurrentProcessId();
#else
    unsigned long pid = getpid();
#endif
    char pidstr[16] = { 0 };
    sprintf(pidstr, "%08x", pid);

    // counter
    char counterstr[16] = {0};
    {
        boost::lock_guard<boost::mutex> lock(mtx);
        ++counter;
        sprintf(counterstr, "%08x", counter);
    }

    value.clear();
    value += std::string(tstr) + std::string(ip) + std::string(pidstr) + std::string(counterstr);
}

}


