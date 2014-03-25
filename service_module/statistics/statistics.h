#ifndef STATISTICS_H_
#define STATISTICS_H_
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "TinyObject.h"
#include "Log.h"
#include "timer.h"

enum  THandleCount
{
    ATOM_ADD_TOTAL = 0,
    ATOM_ADD_SUCC,
    ATOM_ADD_FAIL,
    ATOM_UNKNOW,
};

class QBA_statistics: boost::noncopyable
{
public:
    static QBA_statistics& instance();
    void init(const long  num, boost::asio::io_service& io_service_);
    void  handleCounter(const long idx, const THandleCount&  ht);

private:
    QBA_statistics();
    ~QBA_statistics();
    void  printInfo();

private:
    typedef  boost::shared_ptr<RunFlagCount>  RunFlagCountPTR;
    long   executor_number;
    std::vector<RunFlagCountPTR>   statisticsInfo;
    TinyCount    unknown;
    boost::shared_ptr<Elephants::TimerBase>    timerPtr;
};

#endif


