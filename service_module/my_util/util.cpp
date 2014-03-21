#include "util.h"

namespace Elephants
{
    // TimeFormat
    std::string  TimeFormat::wlocaltime(std::time_t  sec)
    {
        std::tm*  pTime = std::localtime(&sec);
        std::string str;
        try
        {
            str += boost::lexical_cast<std::string>(pTime->tm_year + 1900);

            str += "-";
            if (pTime->tm_mon + 1 < 10)
            {
                str += boost::lexical_cast<std::string>(0);
            }
            str += boost::lexical_cast<std::string>(pTime->tm_mon + 1);

            str += "-";
            if (pTime->tm_mday < 10)
            {
                str += boost::lexical_cast<std::string>(0);
            }
            str += boost::lexical_cast<std::string>(pTime->tm_mday);

            str += " ";
            if (pTime->tm_hour < 10)
            {
                str += boost::lexical_cast<std::string>(0);
            }
            str += boost::lexical_cast<std::string>(pTime->tm_hour);
            str += ":";
            if (pTime->tm_min < 10)
            {
                str += boost::lexical_cast<std::string>(0);
            }
            str += boost::lexical_cast<std::string>(pTime->tm_min);
            str += ":";
            if (pTime->tm_sec < 10)
            {
                str += boost::lexical_cast<std::string>(0);
            }
            str += boost::lexical_cast<std::string>(pTime->tm_sec);
        }
        catch (boost::bad_lexical_cast&)
        {
            return std::ctime(&sec);
        }

        return  str;
    }

    // dateS minus fixed day, dateS's format(20130915)
    bool TimeFormat::jianDay(const std::string& dateS, std::string& dateD, const int fixed)
    {
        if (dateS.length() != 8)
        {
            return false;
        }

        int year = 0;
        int month = 0;
        int day = 0;
        try
        {
            year = boost::lexical_cast<int>(dateS.substr(0,4));
            month = boost::lexical_cast<int>(dateS.substr(4,2));
            day = boost::lexical_cast<int>(dateS.substr(6,2));
        }
        catch (const boost::bad_lexical_cast&)
        {
            return false;
        }

        if (year <= 0 || month <= 0 || month > 12 || day <= 0 || day > 31)
        {
            return false;
        }

        int fixedT = fixed;
        while (fixedT)
        {
            if (day <= fixedT)
            {
                fixedT -= day;

                if (month > 1)
                {
                    --month;
                    day = DAYOFMONTH[month];
                    if (month == 2 && LEAP_YEAR(year))
                    {
                        day += 1;
                    }
                }
                else
                {
                    year -= 1;
                    month = 12;
                    day = 31;
                }
            }
            else
            {
                day -= fixedT;
                fixedT = 0;
                break;
            }
        }

        dateD = MAKE_DATE(year, month, day);
        return true;
    }

};


