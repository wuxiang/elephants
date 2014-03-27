#ifndef TINYOBJECT_H_
#define TINYOBJECT_H_
#include <boost/thread.hpp>
#include <time.h>
#include <string>
#include "Log.h"

class TinyCount
{
public:
    TinyCount();
    long  add_atom();
    long  values();

private:
    boost::mutex  mtx;
    long   seed;
};

class RunFlagCount
{
public:
    RunFlagCount();

    int tAdd();
    int sAdd();
    int fAdd();

    int tValue();
    int sValue();
    int fValue();

private:
    boost::mutex  mtx;
    int total;
    int success;
    int failed;
};

class StrTime
{
public:
    static std::string  tStr();
};

class PrintMessage
{
public:
    //static void printReqMessage(const msgbus::Message& msg);
    //static void print_map(const msgbus::Message::Map &map, int depth);
};

#endif

