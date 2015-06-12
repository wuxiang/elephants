
#ifndef __ODBC_DRIVER_H
#define __ODBC_DRIVER_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/thread/mutex.hpp>


#if defined(_WIN32) || defined(WINDOWS)
#include <winsock2.h>
#include <windows.h>
#endif


#include "SSDBDriver.h"
class otl_connect;
class otl_stream;
class odbc_driver : public ss::db_driver{
public:
    odbc_driver(const char * str) ;
    virtual ~odbc_driver() ;

    bool open(const std::string& param) ;
    void close() ;

    bool execute(const char * sqlstr) ;
    bool query(ss::db_result& result , const char * sqlstr , int limmited = -1) ;

    virtual std::string &real_escape_string(std::string &from) const;
    friend class ss::db_result ;
private:
    otl_connect * conn_ ;
    bool get_header(ss::db_result::header& header , otl_stream &i) const;
    ss::uint8 native_type(int type) const ;
    static boost::mutex m_lock;

    static bool init_;
} ;



#endif  /** __MYSQL_DRIVER_H */
