

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
		������ͬһ�죬����û�б�Ҫ��ʱ���б������ڵ���Ϣ�������ڵ����Ǹ����ǵĴ���
		����ʱ����˵��ÿ��3600*24 = 86400�룬�Ǹ�������

		2011-11-24
		���������գ����Ǳ�ʾ����ʱ�䡣

		����time.h�м�����Ҫ�������͡�
		time(NULL) �����ص�ֵ��ͬ����ʱ�䵽EPOCH��������ͬ
		mktime(struct tm)��Ҳ�ǻ��ڱ���ʱ����м���
		�����ر�ָ���������ʱ�����������ǻ��ڱ���ʱ��
	*/
	class date{
	private:
		time_t time_ ;
		int year_ ;		//��2011
		int month_ ;	//��1...12
		int day_ ;		//����1...31
		int wday_ ;		//��1...7

		std::string datestr_ ;

		/*
			2011-11-23
			���ص���ʱ��0����������������Ϊgmt��������time(NULL)���� ;
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
		��date���ʹ�ã�û�б�Ҫ��ȷ������
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
		stime(); //֧����
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
		���ȿ���΢�룬֧��WIN32 / MINGW / LINUX
		msec ������
		usec ��΢��
		nsec ������
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
		utime(bool need_calc = false) ;//֧��΢��
		~utime() ;

		void reset() ;
		/*
			2011-11-24
			micro�����Ǳ���ʱ�䣬����1,000,000���õ�΢��
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

