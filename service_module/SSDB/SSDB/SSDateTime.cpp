
#include "SSDateTime.h"
#include "SSType.h"

#include <time.h>

#if defined(HAVE_GETTIMEOFDAY)
    #include <sys/time.h>
#endif

#if defined(WIN32) || defined(_WINDOWS) || defined(MINGW32) || defined(MINGW)
#include <windows.h>
#endif


#if defined(MINGW32) || defined(MINGW)

int localtime_s(struct tm * tm , time_t * t)
{
	struct tm *ltm = localtime(t) ;
	if(ltm == NULL)
		return -1 ;
	memcpy(tm , ltm , sizeof(struct tm)) ;
	return 0 ;
}


int gmtime_s(struct tm * tm , time_t * t)
{
	struct tm *ltm = gmtime(t) ;
	if(ltm == NULL)
		return -1 ;
	memcpy(tm , ltm , sizeof(struct tm)) ;
	return 0 ;
}

#endif

namespace ss{

	time_t date::calc_date(time_t time , int& year , int& month , int& day , int& wday)
	{
		struct tm tm ;

	#if defined(WIN32) || defined(_WINDOWS) || defined(MINGW) || defined(MINGW32)
			localtime_s(&tm , &time) ;
	#endif
	
	#if defined(LINUX)
		::localtime_r(&time , &tm) ;
	#endif

		year = tm.tm_year + 1900 ;
		month = tm.tm_mon + 1 ;
		day = tm.tm_mday ;
		wday = tm.tm_wday ;
		if(wday == 0)
			wday = 7 ;

		tm.tm_hour = 0 ;
		tm.tm_min = 0 ;
		tm.tm_sec = 0 ;

		return mktime(&tm) ;
	}

	date::date(int year , int month , int day)
	{
		reset() ;
		time_t t = make_time(year , month , day) ;
		time_ = calc_date(t , year_ , month_ , day_ , wday_) ;
		calc_stage_ = 1 ;	
	}
	
	void date::reset()
	{
		time_ = 0 ;
		year_ = 0 ;
		month_ = 0 ;
		day_ = 0 ;
		wday_ = 0 ;
		calc_stage_ = 0 ;
	}

	time_t date::time()
	{
		return time_ ;
	}

	time_t date::gmttime()
	{
		return (time_ - (date::timezone() * 3600)) ;
	}

	bool date::calc_now()
	{
		time_ = calc_date(::time(NULL) , year_ , month_ , day_ , wday_) ;
		calc_stage_ = 1 ;

		return true ;
	}

	date::date()
	{
		calc_now() ;
	}

	date::date(time_t time)
	{
		reset() ;

		time_ = calc_date(time , year_ , month_ , day_ , wday_) ;
		calc_stage_ = 1 ;	
	}

	date& date::operator=(time_t ts)
	{
		reset() ;
   		time_ = calc_date(ts , year_ , month_ , day_ , wday_) ;	
		calc_stage_ = 1 ;

		return (*this) ;
	}

	bool date::operator==(time_t ts)
	{
		date d(ts) ;
		return (*this) == d ;
	}

	bool date::operator!=(time_t ts)
	{
		date d(ts) ;
		return (*this) != d ;	
	}

	bool date::operator==(const date& d)
	{
		return ((year_ == d.year_) && (month_ == d.month_) && (day_ == d.day_)) ;
	}

	bool date::operator!=(const date& d)
	{
		return !((*this) == d) ;
	}

	bool date::operator<(const date& d)
	{
		return (time_ < d.time_) ;
	}

	bool date::operator<=(const date& d)
	{
		return (time_ <= d.time_) ;
	}

	bool date::operator>(const date& d)
	{
		return (time_ > d.time_) ;
	}

	bool date::operator>=(const date& d)
	{
		return (time_ >= d.time_) ;
	}


	std::string& date::to_string()
	{
		if(calc_stage_ < 2)
		{
			char str[256] = {'\0'} ;
			sprintf(str , "%04d%02d%02d" , year_ , month_ , day_) ;
			datestr_ = str ;

			calc_stage_ = 2 ;
		}

		return datestr_ ;
	}

	date& date::today()
	{
		static date date_today ;
		static time_t last_time = date_today.time() ;

		time_t now = ::time(NULL) ;
		//if((now / 86400) > (last_time / 86400))
		if(now - last_time >= 86400)
		{
			date_today.calc_now() ;
			last_time = date_today.time() ;
		}

		return date_today ;
	}


	int date::timezone()
	{
		static int our_timezone = - 1 ;

		if(our_timezone < 0)
		{
			time_t now = ::time(NULL) ;
			struct tm ltm , gtm ;

#if defined(WIN32) || defined(_WINDOWS) || defined(MINGW) || defined(MINGW32)
			localtime_s(&ltm , &now) ;
			gmtime_s(&gtm , &now) ;
#else
			localtime_r(&now , &ltm) ;
			gmtime_r(&now , &gtm) ;
#endif

			our_timezone = (ltm.tm_hour + 24 - gtm.tm_hour) %24 ;
		}

		return our_timezone ;
	}

	date& date::next_day(int dc)
	{
		time_t t = time_ + (86400 * dc) ;
		time_ = calc_date(t , year_ , month_ , day_ , wday_) ;	

		calc_stage_ = 1 ;
		return (*this) ;
	}


	stime::stime()
	{
		reset() ;
	}

	stime::stime(time_t ts)
	{
		reset() ;
		calc_now(ts) ;
	}

	stime::stime(const stime& st)
	{
		hour_ = st.hour_ ;
		minute_ = st.minute_ ;
		second_ = st.second_ ;

		secstr_ = st.secstr_ ;
		calc_stage_ = st.calc_stage_ ;
	}

	stime::~stime()
	{
		//
	}

	void stime::reset()
	{
		hour_ = 0 ;
		minute_ = 0 ;
		second_ = 0 ;

		secstr_ = "" ;
		calc_stage_ = 0 ;
	}

	void stime::calc_now(ss::int64 sec)
	{
		time_t s = 0 ;
		if(sec == 0)
		{
			s = ::time(NULL) ;
		}
		else
		{
			s = (time_t)sec ;
		}

		if(s > 864000)
		{
			ss::date d(s) ;
			s -= d.time() ;
		}

		hour_ = (int)(s / 3600) ;
		minute_ = (int)((s % 3600) / 60) ;
		second_ = (int)(s % 60) ;

		calc_stage_ = 1 ;	
	}

	const std::string& stime::to_string()
	{
		if(calc_stage_ < 2)
		{
			char str[256] ;
			sprintf(str , "%02d:%02d:%02d" , hour_ , minute_ , second_) ;
			secstr_ = str ;

			calc_stage_ = 2 ;
		}

		return secstr_ ;
	}

	const stime& stime::operator=(const stime& st)
	{
		hour_ = st.hour_ ;
		minute_ = st.minute_ ;
		second_ = st.second_ ;

		secstr_ = st.secstr_ ;
		calc_stage_ = st.calc_stage_ ;

		return (*this) ;		
	}

	time_t stime::time()
	{
		return (hour_ * 3600 + minute_ * 60 + second_) ;
	}

	ss::int64 stime::now()
	{
		ss::stime stime(::time(NULL)) ;
		return stime.time() ;
	}

	utime::utime(bool need_calc)
	{
		if(need_calc)
			calc_now() ;
		else
			reset() ;
	}

	void utime::reset()
	{
		hour_ = 0 ;
		minute_ = 0 ;
		second_ = 0 ;
		usec_ = 0 ;

		calc_stage_ = 0 ;
	}

	utime::~utime()
	{
		//
	}

	void utime::calc_now(ss::int64 micro)
	{
		time_t sec = 0 ;	
		if(micro == 0)
		{
	#if defined(HAVE_GETTIMEOFDAY)
			struct timeval tv ;
			::gettimeofday(&tv , NULL) ;

			sec = tv.tv_sec ;
			usec_ = tv.tv_usec ;
	#else
			FILETIME ft ;

			GetSystemTimeAsFileTime(&ft) ;

			/*
				2011-03-08
				参考BOOST的microsec_time_clock.hpp	将FILETIME转化为毫秒

				shift is difference between 1970-Jan-01 & 1601-Jan-01
				in 100-nanosecond intervals 
			*/
			const uint64 shift = 116444736000000000ULL; // (27111902 << 32) + 3577643008
			ss::uint64 caster = ft.dwHighDateTime  ;
			caster = (( caster << 32 ) + ft.dwLowDateTime - shift) / 10 ;

			usec_ = (ss::int64)(caster % 1000000UL) ;
			sec = (ss::int64)(caster  / 1000000) ;

	#endif
		}
		else
		{
			sec = (time_t)(micro / 1000000) ;
			usec_ = (int)(micro % 1000000) ;
		}

		if(sec > 864000)
		{
			ss::date d(sec) ;
			sec -= d.time() ;
		}

		hour_ = (int)(sec / 3600) ;
		minute_ = (int)((sec % 3600) / 60) ;
		second_ = (int)(sec % 60) ;		

		calc_stage_ = 1;
	}

	std::string& utime::to_string()
	{
		if(calc_stage_ < 2)
		{
			char str[256] ;
			sprintf(str , "%02d:%02d:%02d.%06d" , hour_ , minute_ , second_ , usec_) ;
			usecstr_ = str ;		
		}

		return usecstr_ ;
	}

	ss::int64 utime::now()
	{
		ss::utime ut(true) ;
		return (((ut.hour() * 3600 + ut.minute() * 60 + ut.second())) * 1000000 + ut.usec()) ;
	}

	const utime& utime::operator=(const utime& src)
	{
		hour_ = src.hour_ ;
		minute_ = src.minute_ ;
		second_ = src.second_ ;
		usec_ = src.usec_ ;

		usecstr_ = src.usecstr_ ;
		calc_stage_ = src.calc_stage_ ;

		return (*this) ;	
	}
}

