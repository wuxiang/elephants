#ifndef UUID_H_
#define UUID_H_
#include <string>
#include <time.h>
#include <cstdlib>
#include <boost/thread.hpp>


namespace Elephants
{
    // 4 bytes,a 4-byte value representing the seconds since the Unix epoch
    // 4 bytes,a 4-byte machine identifier
    // 4 a 2-byte process id, and
    // 4 a 3-byte counter, starting with a random value.
    class UUID
    {
    public:
        UUID();
        UUID(const UUID&  id);
        UUID operator=(const UUID& id);
        bool operator<(const UUID& id);
        bool operator==(const UUID& id);
        std::string get_id();

    private:
        struct IPV4
        {
            unsigned char b1;           
            unsigned char b2;
            unsigned char b3;
            unsigned char b4;
            IPV4(): b1(0), b2(0), b3(0), b4(0)
            {
            }
            IPV4(const IPV4& ip)
            {
                b1 = ip.b1;
                b2 = ip.b2;
                b3 = ip.b3;
                b4 = ip.b4;
            }
            IPV4 operator=(const IPV4& ip)
            {
                b1 = ip.b1;
                b2 = ip.b2;
                b3 = ip.b3;
                b4 = ip.b4;
            }
        };

    private:
        void generator();

        // get machine identerfy
        bool getIP(IPV4& ip);
    private:
        std::string  value;

        static boost::mutex mtx;
        static int counter;
    };
}

#endif

