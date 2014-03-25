#ifndef STATISTICS_H_
#define STATISTICS_H_
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <map>

namespace Elephants 
{
    class  Timer_micro_surive
    {
    public:
        Timer_micro_surive();
        virtual ~Timer_micro_surive();
        virtual void event();
    private:
        boost::posix_time::ptime  begin;
        std::size_t    elapsed;  // microsec
    };

    class Statistics_Base
    {
    public:
    protected:
    };
}

#endif

