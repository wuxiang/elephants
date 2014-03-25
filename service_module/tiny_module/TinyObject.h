#ifndef TINYOBJECT_H_
#define TINYOBJECT_H_
#include <boost/thread.hpp>
#include <time.h>
#include <string>
#include "msgbus/variant.h"
#include "msgbus/message.h"
#include "Log.h"

class TinyCount
{
public:
    TinyCount();
    msgbus::int64_t  add_atom();
    msgbus::int64_t  values();

private:
    boost::mutex  mtx;
    msgbus::int64_t   seed;
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
    static void printReqMessage(const msgbus::Message& msg);
    static void print_map(const msgbus::Message::Map &map, int depth);
};

#endif

