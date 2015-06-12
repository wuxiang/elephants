
#ifndef __SS_LOG_H
#define __SS_LOG_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "SSDateTime.h"
#include "SSThread.h"

namespace ss {

	class ssdb_logger {
	protected:
		FILE *file_ ;
		int level_ ;
		int tty_level_ ;
		date today_ ;

		std::string name_ ;
		std::string file_name_ ;

		size_t max_size_ ;

		void init() ;
		void open() ;
		void close() ;

		/*
			2012-02-07
			用来处理，凌晨时，正在写文件，但去关闭文件引起的问题
		*/
		shared_mutex guard_ ;
		void write_message(const char * message) ;
	public:
		ssdb_logger() ;
		ssdb_logger(const char *name) ;	

		~ssdb_logger() ;

		int set_level(int level) ;
		int set_stdout_level(int level) ;
		int set_tty_level(int level) ;
		int get_level() const ;
		int set_max_size(size_t max_size) ;
		int write(const char *file_name , int line , int level , const char *format , ...) ;
		int writebuf(const char *file_name , int line , int level , const void *buf , int size) ;
	} ;

	namespace LOG_LEVEL{
		const int LOG_EMERG			=   0 ;
		const int LOG_ALERT			=   1 ;
		const int LOG_CRIT			=   2 ;
		const int LOG_ERR			=   3 ;
		const int LOG_WARN			=   4 ;
		const int LOG_NOTICE		=   5 ;
		const int LOG_INFO			=   6 ;
		const int LOG_DEBUG			=   7 ;	
		const int LOG_LEVEL_MAX    =   LOG_DEBUG ;
	}
} ;

#ifdef __cplusplus
extern "C" {
#endif

	ss::ssdb_logger * get_my_logger() ;

#ifdef __cplusplus
}
#endif

#define DEFAULT_LOGGER get_my_logger()

#define LOGWRITE(level , format , ...) \
	do{									\
		if(DEFAULT_LOGGER->get_level() < level) \
			break ;								\
		DEFAULT_LOGGER->write(__FILE__ , __LINE__ , level , format , ##__VA_ARGS__);	\
	}while(0)
#define LOGEMERG(format , ...) LOGWRITE(ss::LOG_LEVEL::LOG_EMERG , format , ##__VA_ARGS__) 
#define LOGALERT(format , ...) LOGWRITE(ss::LOG_LEVEL::LOG_ALERT , format , ##__VA_ARGS__) 
#define LOGCRIT(format , ...) LOGWRITE(ss::LOG_LEVEL::LOG_CRIT , format , ##__VA_ARGS__) 
#define LOGERR(format , ...) LOGWRITE(ss::LOG_LEVEL::LOG_ERR , format , ##__VA_ARGS__) 
#define LOGWARN(format , ...) LOGWRITE(ss::LOG_LEVEL::LOG_WARN , format , ##__VA_ARGS__) 
#define LOGNOTICE(format , ...) LOGWRITE(ss::LOG_LEVEL::LOG_NOTICE , format , ##__VA_ARGS__) 
#define LOGINFO(format , ...) LOGWRITE(ss::LOG_LEVEL::LOG_INFO , format , ##__VA_ARGS__) 
#define LOGDEBUG(format , ...) LOGWRITE(ss::LOG_LEVEL::LOG_DEBUG , format , ##__VA_ARGS__) 

#define LOGBUFWRITE(level , buf , size)		\
	do{										\
		if(DEFAULT_LOGGER->get_level() < level) \
			break ;								\
		DEFAULT_LOGGER->writebuf(__FILE__ , __LINE__ , level , buf , size);	\
	}while(0)
#define LOGBUFEMERG(buf , size) LOGBUFWRITE(ss::LOG_LEVEL::LOG_EMERG , buf , size) 
#define LOGBUFALERT(buf , size) LOGBUFWRITE(ss::LOG_LEVEL::LOG_ALERT , buf , size) 
#define LOGBUFCRIT(buf , size) LOGBUFWRITE(ss::LOG_LEVEL::LOG_CRIT , buf , size) 
#define LOGBUFERR(buf , size) LOGBUFWRITE(ss::LOG_LEVEL::LOG_ERR , buf , size) 
#define LOGBUFWARN(buf , size) LOGBUFWRITE(ss::LOG_LEVEL::LOG_WARN , buf , size) 
#define LOGBUFNOTICE(buf , size) LOGBUFWRITE(ss::LOG_LEVEL::LOG_NOTICE , buf , size) 
#define LOGBUFINFO(buf , size) LOGBUFWRITE(ss::LOG_LEVEL::LOG_INFO , buf , size) 
#define LOGBUFDEBUG(buf , size) LOGBUFWRITE(ss::LOG_LEVEL::LOG_DEBUG , buf , size) 

#define MAX_LOG_LEN 4096

#endif  /** __SS_LOG_H */
