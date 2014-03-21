#ifndef MY_UTIL_H_
#define MY_UTIL_H_
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <ctime>
#include <string>

const char* const STR_MONTH[] = {
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12"
};
const char* const STR_DAY[] = {
    "01", "02", "03", "04",	"05", "06", "07", "08", "09", "10",
    "11", "12", "13", "14",	"15", "16", "17", "18", "19", "20",
    "21", "22", "23", "24",	"25", "26", "27", "28", "29", "30",
    "31"
};
const int DAYOFMONTH[13] = {-1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#define YEAR(d) (boost::lexical_cast<int>(d.substr(0,4)))
#define MONTH(d) (boost::lexical_cast<int>(d.substr(4,2)))
#define DAY(d) (boost::lexical_cast<int>(d.substr(6,2)))

#define HOUR(d) (boost::lexical_cast<int>(d.substr(0,2)))
#define MINTER(d) (boost::lexical_cast<int>(d.substr(2,2)))
#define SECOND(d) (boost::lexical_cast<int>(d.substr(4,2)))
#define LEAP_YEAR(y) (((y)%4==0&&(y)%100!=0)||((y)%400==0))
#define MAKE_DATE(y,m,d) (boost::lexical_cast<std::string >(y) + STR_MONTH[m-1] + STR_DAY[d - 1])

#define HOURISVAILD(d) (d >= 0 && d < 24)
#define MINISVALID(d) (d >= 0 && d < 60)
#define SECISVALID(d) (d >= 0 && d < 60)

namespace Elephants
{
    class  TimeFormat
    {
    public:
        // transform time(NULL) to string format(xxxx-xx-xx xx:xx:xx)
        static std::string  wlocaltime(std::time_t  sec);
        static bool jianDay(const std::string& dateS, std::string& dateD, const int fixed);
    };

};

#endif


