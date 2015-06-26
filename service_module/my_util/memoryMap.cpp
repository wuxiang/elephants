#include "util.h"


std::string  BrokerUtil::convertDealTime(const std::string& logtime)
{
    if (logtime.length() < 14)
    {
        DA_LOG("process", Elephants::ERR, "Error[%s][%d]: error LogTime[%s]", logtime.c_str());
        return logtime;
    }

    std::string val = logtime.substr(0, 14);

    std::tm timeinfo;
    std::memset(&timeinfo,0, sizeof(timeinfo));
    sscanf(val.c_str(),"%4d%2d%2d%2d%2d%2d",&timeinfo.tm_year,&timeinfo.tm_mon,&timeinfo.tm_mday,&timeinfo.tm_hour,&timeinfo.tm_min,&timeinfo.tm_sec);

    timeinfo.tm_year -= 1900;
    timeinfo.tm_mon--;

    std::time_t clock = mktime(&timeinfo);
    
    return boost::lexical_cast<std::string>(clock);
}

std::string  BrokerUtil::converDate(const std::string& logtime)
{
    if (logtime.length() < 6)
    {
        DA_LOG("process", Elephants::ERR, "Error[%s][%d]: invalid time[%s] 6", __FUNCTION__, __LINE__, logtime.c_str());
        return logtime;
    }
    std::string year = logtime.substr(0, 4);
    std::string mon = logtime.substr(4,2);
    std::string day = logtime.substr(6,2);

    return year + "-" + mon + "-" + day;
}


