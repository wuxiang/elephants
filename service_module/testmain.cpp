#include <iostream>
#include <signal.h>
#include "util.h"
#include "io_service_pool.h"
#include "timer.h"
#include "statistics_base.h"
#include "signal_system.h"
#include "uuid.h"

void print(const std::string value)
{
    std::cout << value << std::endl;
}


void ternal_process(Elephants::io_service_pool* pool, const int signum)
{
    std::cout << "get signal[" << signum << "]" << std::endl;
    pool->stop();
}


int main()
{
    Elephants::UUID  id;
    std::string val = id.get_id();
    std::size_t len = val.size();
    std::cout << "++++++++++++++++++++++++++++++work start++++++++++++++++++++++" << std::endl;
    Elephants::io_service_pool  pool(1);
    {
        Elephants::Timer_micro_surive  tt;
        
        Sleep(2);
    }
    std::cout << GetCurrentProcessId() << std::endl;
    Elephants::SignalSystem   signal_machine(pool.get_io_service());
    signal_machine.registerEvent(SIGINT, boost::bind(ternal_process, &pool, _1));

    Elephants::PeriodTimer  timer(pool.get_io_service(), 5, boost::bind(print, std::string("hello world")));
    timer.wait_async();
   
    Elephants::PointerTimer  ptimer(pool.get_io_service(), std::string("10:50:00"), boost::bind(print, std::string("i am coming")));
    ptimer.wait_async();
    pool.run();
    std::cout << "++++++++++++++++++++++++++++++work end++++++++++++++++++++++" << std::endl;
    return 0;
}


