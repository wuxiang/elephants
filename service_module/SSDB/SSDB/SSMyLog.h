/* 
   SSMyLog.h
   Copyright (C) 

  Êä³ö¸÷Àà Log
*/
#ifndef SSMYLOG_H
#define SSMYLOG_H

#include <list>
#include <string>
#include <stdlib.h>
#include <fstream>
#include "SSDateTime.h"
#include <boost/thread/mutex.hpp>

#define DA_LOG(...) SSMyLog::Instance()->Output(__VA_ARGS__)
#define DA_ERRLOG(...) SSMyLog::Instance()->OutputErr(__VA_ARGS__)
#define DA_FILELOG(...) SSMyLog::Instance()->OutputToFile(__VA_ARGS__)
#define DA_FLUSH(...) SSMyLog::Instance()->flushIt(__VA_ARGS__)

#define LOG_FLUSH 0
#define ERROR_FLUSH 1
#define BINARY_FLUSH 2
#define DATA_FLUSH 3

class SSMyLog
{
public:
    static SSMyLog *Instance();

    void Output(const char*,...);
    void Output(std::string&);

    void OutputErr(const char*,...);
    void OutputErr(std::string&);

    void BinaryOutput(const char*, int len);

    void OutputToFile(const std::string&);

    void flushIt(int obj);
protected:
    ss::date today_;
private:
    SSMyLog(){};
    virtual ~SSMyLog();
    SSMyLog(const SSMyLog&);

    std::string getFileName(const char *filepath);


    static SSMyLog instance_;

    std::list<std::string> strbuf_queue_;

    std::ofstream binaryfile_;
    std::ofstream errorfile_;
    std::ofstream logfile_;
    std::ofstream datafile_;

    boost::mutex    lock_;

};


#endif
