

#ifndef __SS_DATETIME_H
#define __SS_DATETIME_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <string>

#include "SSType.h"

namespace ss{

	/*
		由于是同一天，所以没有必要在时间中保存日期的信息，将日期单独是个明智的处理。
		对于时间来说，每天3600*24 = 86400秒，是个常量。

		2011-11-24
		这里年月日，都是表示本地时间。

		关于time.h中几个重要函数解释。
		time(NULL) ；返回的值，同本地时间到EPOCH的秒数相同
		mktime(struct tm)；也是基于本地时间进行计算
		除非特别指出，这里的时间秒数，都是基于本地时间
	*/
	class date{
	private:
		time_t time_ ;
		int year_ ;		//年2011
		int month_ ;	//月1...12
		int day_ ;		//日期1...31
		int wday_ ;		//周1...7

		std::string datestr_ ;

		/*
			2011-11-23
			返回当地时间0点的秒数，输入参数为gmt秒数，由time(NULL)返回 ;
		*/
		time_t calc_date(time_t time , int& year , int& month , int& day , int& wday) ;

		int calc_stage_ ;
	public:
		date() ;
		date(time_t ts) ;
		date(int year , int month , int day) ;
		void reset() ;

		date& operator=(time_t ts) ;
		bool operator==(time_t ts) ;
		bool operator!=(time_t ts) ;

		bool operator==(const date& d) ;
		bool operator!=(const date& d) ;

		bool operator<(const date& d) ;
		bool operator<=(const date& d) ;
		bool operator>(const date& d) ;
		bool operator>=(const date& d) ;

		std::string& to_string() ;
		time_t time() ;
		time_t gmttime() ;
		bool calc_now() ;

		inline int year() const {return year_ ;}
		inline int month() const {return month_ ;}
		inline int day() const {return day_ ;}
		inline int wday() const {return wday_ ;}

		date& next_day(int dc = 1) ;

		static date& today() ;
		static int timezone() ;
	} ;

	/*
		和date配合使用，没有必要精确到毫秒
	*/
	class stime{
	protected:
		int hour_ ;
		int minute_ ;
		int second_ ;

		std::string secstr_ ;
		void calcutestr() ;
		int calc_stage_ ;
	public:
		stime(); //支持秒
		stime(time_t ts) ;
		stime(const stime& st) ;
		~stime() ;

		void reset(); 
		void calc_now(ss::int64 sec = 0) ;

		inline int hour() const {return hour_ ;}
		inline int minute() const {return minute_ ;}
		inline int second() const {return second_ ;}

		time_t time() ;
		const std::string& to_string()  ;
		const stime& operator=(const stime& st) ;

		static ss::int64 now() ;	
	} ;


	/*
		精度控制微秒，支持WIN32 / MINGW / LINUX
		msec ：毫秒
		usec ：微秒
		nsec ：纳秒
	*/

	class utime{
	protected:
		int hour_ ;
		int minute_ ;
		int second_ ;
		int usec_ ;

		std::string usecstr_ ;

		void calcutestr() ;
		int calc_stage_ ;
	public:
		utime(bool need_calc = false) ;//支持微秒
		~utime() ;

		void reset() ;
		/*
			2011-11-24
			micro必须是本地时间，乘以1,000,000。得到微秒
		*/
		void calc_now(ss::int64 micro = 0) ;

		inline int hour() const {return hour_ ;}
		inline int minute() const {return minute_ ;}
		inline int second() const {return second_ ;}
		inline int usec() const {return usec_ ;}

		std::string& to_string() ;

		static ss::int64 now() ;
		const utime& operator=(const utime& src) ;
	} ;
}

#endif /**  __SS_DATETIME_H */

