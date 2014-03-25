#include "statistics.h"

namespace Elephants
{
    Timer_micro_surive::Timer_micro_surive()
    {
        begin = boost::posix_time::microsec_clock::local_time();
        std::cout << boost::posix_time::to_simple_string(begin) << std::endl;
    }

    Timer_micro_surive::~Timer_micro_surive()
    {
        boost::posix_time::ptime end = boost::posix_time::microsec_clock::local_time();
        std::cout << boost::posix_time::to_simple_string(end) << std::endl;

        boost::posix_time::time_duration peroid = end - begin;
        elapsed = peroid.total_microseconds();
        event();
    }

    void Timer_micro_surive::event()
    {
        std::cout << elapsed << std::endl;
    }

}


