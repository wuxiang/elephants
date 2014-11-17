#ifndef CALLBACK_H_
#define CALLBACK_H_
#include <boost/noncopyable.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp> 
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>  
#include <boost/iostreams/device/file.hpp>

#include "msgbus/connection.h"

class Call: public msgbus::Callback, public boost::noncopyable
{
public:
    static Call& instance();
    bool init();

private:
    Call();
    ~Call();
    virtual void  message(const msgbus::Message& msg);

private:
    msgbus::Connection  m_connect;
};

#endif