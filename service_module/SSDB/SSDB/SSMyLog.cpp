/* 
   SSMyLog.h
   Copyright (C) 

  Êä³ö¸÷Àà Log
*/

#include "SSMyLog.h"
#include "SSDateTime.h"
#include <stdarg.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <locale>

using namespace std;
using namespace boost::filesystem;

#define AUTOSYNC(a) boost::mutex::scoped_lock lock(a)

static const char RAWDATA_FOLDER[] = "ss_rawdata";
static const char ERROR_FOLDER[] = "ss_error";
static const char LOG_FOLDER[] = "ss_log";
static const char FILE_FOLDER[] = "ss_data";
//static const int MAX_FILE_SIZE = 20 * 1024 * 1024;

SSMyLog SSMyLog::instance_;

SSMyLog*
SSMyLog::Instance()
{
    return &instance_;
}

SSMyLog::~SSMyLog()
{
}

void
SSMyLog::Output(const char* fmt,...)
{
    AUTOSYNC(lock_);
    int size=100;
    string str;
    while (1) {
        str.resize(size);
        va_list ap;
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.c_str(), size, fmt, ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize(n);
            break;
        }
        else{
            size *=2;
        }
    }
    Output(str);
}

void
SSMyLog::Output(std::string& str)
{
    time_t timeval;
    const tm    *timeinfo;

    timeval = time(NULL);  
    timeinfo = localtime(&timeval);

    char time[50];
    sprintf(time,"[%04d%02d%02d_%02d:%02d:%02d]",
                timeinfo->tm_year + 1900,
                timeinfo->tm_mon + 1,
                timeinfo->tm_mday,
                timeinfo->tm_hour,
                timeinfo->tm_min,
                timeinfo->tm_sec
                );
    cout << time << str << '\n';

    if(logfile_.is_open()){
        if (today_ != ss::date::today()){
            logfile_.flush();
            std::locale old_loc = std::locale::global(std::locale(""));
            logfile_.close();
            logfile_.open(getFileName(LOG_FOLDER).c_str(),ios::out|ios::app);
            std::locale::global(old_loc);
            today_.calc_now();
        }
    }
    else{
        std::locale old_loc = std::locale::global(std::locale(""));
        logfile_.open(getFileName(LOG_FOLDER).c_str(),ios::out|ios::app);
        std::locale::global(old_loc);
        today_.calc_now();
    }
    if(logfile_.fail()){
        printf("[%s]file open failure (%s)",__FUNCTION__,getFileName(LOG_FOLDER).c_str());
        return;
    }
    logfile_ << time << str << '\n';
}

#if 0
std::string
SSMyLog::toString(const char* fmt,va_list ap){
    int size=100;
    string str;
    while (1) {
        str.resize(size);
        int n = vsnprintf((char *)str.c_str(), size, fmt, ap);
        if (n > -1 && n < size) {
            str.resize(n);
            break;
        }
        else{
            size *=2;
        }
    }
    return str;
}
#endif
void 
SSMyLog::OutputErr(const char* fmt,...)
{
    AUTOSYNC(lock_);
    int size=100;
    string str;
    while (1) {
        str.resize(size);
        va_list ap;
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.c_str(), size, fmt, ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize(n);
            break;
        }
        else{
            size *=2;
        }
    }
    OutputErr(str);
}

void
SSMyLog::OutputErr(std::string& str)
{
    time_t timeval;
    const tm    *timeinfo;

    timeval = time(NULL);  
    timeinfo = localtime(&timeval);

    char time[50];
    sprintf(time,"[%04d%02d%02d_%02d:%02d:%02d]",
                timeinfo->tm_year + 1900,
                timeinfo->tm_mon + 1,
                timeinfo->tm_mday,
                timeinfo->tm_hour,
                timeinfo->tm_min,
                timeinfo->tm_sec
                );
    cerr << time << str << '\n';
    if(errorfile_.is_open()){
        if(today_ != ss::date::today()){
            errorfile_.flush();
            std::locale old_loc = std::locale::global(std::locale(""));
            errorfile_.close();
            errorfile_.open(getFileName(ERROR_FOLDER).c_str(),ios::out|ios::app);
            today_.calc_now();
            std::locale::global(old_loc);
        }
    }
    else{
        std::locale old_loc = std::locale::global(std::locale(""));
        errorfile_.open(getFileName(ERROR_FOLDER).c_str(),ios::out|ios::app);
        today_.calc_now();
        std::locale::global(old_loc);
    }
    if(errorfile_.fail()){
        cerr << "[" << __FUNCTION__ << "]file open failure (" << getFileName(ERROR_FOLDER) << ")" << endl;
        return;
    }
    errorfile_ << time << str <<'\n';
}

void 
SSMyLog::BinaryOutput(const char* buf, int len)
{
    AUTOSYNC(lock_);
    if(binaryfile_.is_open()){
        if(today_ != ss::date::today()){
            binaryfile_.flush();
            std::locale old_loc = std::locale::global(std::locale(""));
            binaryfile_.close();
            binaryfile_.open(getFileName(RAWDATA_FOLDER).c_str(),ios::binary|ios::out);
            today_.calc_now();
            std::locale::global(old_loc);
        }
    }
    else{
        std::locale old_loc = std::locale::global(std::locale(""));
        binaryfile_.open(getFileName(RAWDATA_FOLDER).c_str(),ios::binary|ios::out);
        today_.calc_now();
        std::locale::global(old_loc);
    }
    if(binaryfile_.fail()){
        DA_ERRLOG("[%s]file open failure (%s)",__FUNCTION__,getFileName(RAWDATA_FOLDER).c_str());
        return;
    }
    binaryfile_.write(buf,len);
}

void
SSMyLog::OutputToFile(const std::string& str)
{
    AUTOSYNC(lock_);
    if(datafile_.is_open()){
        if(today_ != ss::date::today()){
            datafile_.flush();
            std::locale old_loc = std::locale::global(std::locale(""));
            datafile_.close();
            datafile_.open(getFileName(FILE_FOLDER).c_str(),ios::binary|ios::out|ios::app);
            today_.calc_now();
            std::locale::global(old_loc);
        }
    }
    else{
        std::locale old_loc = std::locale::global(std::locale(""));
        datafile_.open(getFileName(FILE_FOLDER).c_str(),ios::binary|ios::out|ios::app);
        today_.calc_now();
        std::locale::global(old_loc);
    }
    if(datafile_.fail()){
        DA_ERRLOG("[%s]file open failure (%s)",__FUNCTION__,getFileName(FILE_FOLDER).c_str());
        return;
    }
    time_t timeval;
    const tm    *timeinfo;

    timeval = time(NULL);  
    timeinfo = localtime(&timeval);

    char time[50];
    sprintf(time,"[%04d%02d%02d_%02d:%02d:%02d]",
        timeinfo->tm_year + 1900,
        timeinfo->tm_mon + 1,
        timeinfo->tm_mday,
        timeinfo->tm_hour,
        timeinfo->tm_min,
        timeinfo->tm_sec
        );
    datafile_ << time << str << '\n';
}

std::string
SSMyLog::getFileName(const char *filepath)
{

    time_t timeval;
    const tm    *timeinfo;
    timeval = time(NULL);  
    timeinfo = localtime(&timeval);
    char time[50];
    sprintf(time,"%04d%02d%02d",
                timeinfo->tm_year + 1900,
                timeinfo->tm_mon + 1,
                timeinfo->tm_mday
                );
    ostringstream filename;
    filename << time;

    path new_path = current_path();
    new_path /= filepath;

    if(!exists(new_path)){
        create_directory(new_path);
    }
    new_path/=filename.str();

    return new_path.string();
}

void
SSMyLog::flushIt(int obj){
    AUTOSYNC(lock_);
    switch(obj){
        case LOG_FLUSH:
                logfile_.flush();
                break;
        case ERROR_FLUSH:
                errorfile_.flush();
                break;
        case BINARY_FLUSH:
                binaryfile_.flush();
                break;
        case DATA_FLUSH:
                datafile_.flush();
                break;
        default:
                break;
    }
}

/*bool SSMyLog::IsFileNameUpdate(ostream &of)
{
    std::streampos cur = of.tellp();
    std::streampos filesize;

    of.seekp(0,ios::end);
    filesize = of.tellp();
    of.seekp(cur);
    //return (filesize > MAX_FILE_SIZE);
    return (today_ == date::today());
}*/
