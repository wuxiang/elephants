
#ifndef __SS_DB_DRIVER_H
#define __SS_DB_DRIVER_H 1

#include "SSTuple.h"
#include "SSRList.h"
#include "SSField.h"
#include "SSDBResult.h"

#include <string>
#include <map>

namespace ss {
	class db_driver{
	protected:
		std::string name_ ;
		std::string errmsg_ ;
		int errcode_ ;

		std::string host_ ;
		std::string port_ ;
		std::string username_ ;
		std::string password_ ;
		std::string database_ ;

	public:
		db_driver(const char * str) ;
		virtual ~db_driver() ;

		inline const std::string& name() const {return name_ ;}
		inline const std::string& errmsg() const{return errmsg_ ;}
		inline int errcode() const {return errcode_ ;}
		inline const std::string& database() const{return database_;}

		/*
			2012-06-20
			连接参数，以':'分隔
			参数格式 host:port:username:password:database
		*/
		virtual bool open(const std::string& param) ;
		virtual void close() ;

		virtual void split_param(const std::string& param) ;

		virtual bool execute(const char * sqlstr) ;

		/*
			2012-06-25
			result里面如果有头部定义，那么以该头部定义为主，否则以sql返回的头部定义为准。
			只支持按位绑定
		*/
		virtual bool query(db_result& result , const char * sqlstr , int limmited = -1) ;

		virtual bool begin() ;
		virtual bool rollback() ;
		virtual bool commit() ;

		virtual std::string &real_escape_string(std::string &from) const;

		static db_driver * create(const std::string& name) ;
		static db_driver * create() ;
	} ;
}


#endif  /** __SS_DB_DRIVER_H */
