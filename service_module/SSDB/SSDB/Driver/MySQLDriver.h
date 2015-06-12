
#ifndef __MYSQL_DRIVER_H
#define __MYSQL_DRIVER_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/thread/mutex.hpp>


#if defined(_WIN32) || defined(WINDOWS)
	#include <winsock2.h>
	#include <windows.h>
#endif


#include <mysql.h>
#include "SSDBDriver.h"

class mysql_driver : public ss::db_driver{
public:
	mysql_driver(const char * str) ;
	virtual ~mysql_driver() ;

	bool open(const std::string& param) ;
	void close() ;

	bool execute(const char * sqlstr) ;
	bool query(ss::db_result& result , const char * sqlstr , int limmited = -1) ;

	ss::uint8 native_type(enum_field_types type) ;

	virtual std::string &real_escape_string(std::string &from) const;
	friend class ss::db_result ;
private:
	MYSQL * conn_ ;
	static boost::mutex m_lock;
	bool inner_query(const char * sqlstr , MYSQL_RES *& result ,ss::uint64& rc , ss::uint32& fc) ;

	bool print_mysql_row(ss::db_result::header& header , int index , MYSQL_ROW row , char *  buffer , int buflen) ;

	bool get_header(ss::db_result::header& header , MYSQL_RES* res) ;
	
	static bool init_;
} ;



#endif  /** __MYSQL_DRIVER_H */
