#include "statistics.h"
#include "executor.h"

QBA_statistics& QBA_statistics::instance()
{
    static  QBA_statistics  handler;
    return handler;
}

void QBA_statistics::init(const long  num, boost::asio::io_service& io_service_)
{
    executor_number = num;
    for (int i = 0; i < num; ++i)
    {
        statisticsInfo.push_back(RunFlagCountPTR(new (std::nothrow) RunFlagCount()));
    }
    // register timer

    timerPtr.reset(new (std::nothrow) Elephants::PeriodTimer(io_service_,  200, boost::bind(&QBA_statistics::printInfo, this)));
    timerPtr->wait_async();
}

void  QBA_statistics::handleCounter(const long idx, const THandleCount&  ht)
{
    if (ht == ATOM_ADD_TOTAL)
    {
        (statisticsInfo[idx])->tAdd();
    }
    else if (ht == ATOM_ADD_SUCC)
    {
        (statisticsInfo[idx])->sAdd();
    }
    else if (ht == ATOM_ADD_FAIL)
    {
        (statisticsInfo[idx])->fAdd();
    }
    else
    {
        unknown.add_atom();
    }
}

QBA_statistics::QBA_statistics()
{

}

QBA_statistics::~QBA_statistics()
{

}

void  QBA_statistics::printInfo()
{
    try
    {
        std::string  info;
        info.assign("[runing information]\n");
        for (int i = 0; i < executor_number; ++i)
        {
            int total = statisticsInfo[i]->tValue();
            int succ = statisticsInfo[i]->sValue();
            int fail = statisticsInfo[i]->fValue();
            info += "executor " + boost::lexical_cast<std::string>(i) + ":  ";
            info += "total[" + boost::lexical_cast<std::string>(total) + "], ";
            info += "succ[" + boost::lexical_cast<std::string>(succ) + "], ";
            info += "failed[" + boost::lexical_cast<std::string>(fail) + "]\n";
        }
        info += "unknown[" + boost::lexical_cast<std::string>(unknown.values()) + "]\n\n";
        DA_LOG(info);
    }
    catch (const boost::bad_lexical_cast&)
    {
    	return;
    }
    catch (...)
    {
        return;
    }
}


