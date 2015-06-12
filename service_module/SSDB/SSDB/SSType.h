
#ifndef __SS_TYPE_H
#define __SS_TYPE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
#else
#include <stdint.h>
#endif

namespace ss {

typedef char                int8    ;
typedef unsigned char       uint8   ;
typedef short int           int16   ;
typedef unsigned short int  uint16  ;
typedef int                 int32   ;
typedef unsigned int        uint32  ;

#ifdef WIN32
typedef __int64             int64   ;
typedef unsigned __int64    uint64  ;

#define atoi64  _atoi64

#else

typedef int64_t             int64   ;
typedef uint64_t            uint64  ;

#if __WORDSIZE == 64
#define atoi64  atol
#else 
#define atoi64  atoll
#endif

#endif

    const uint8     SSTYPE_MIN      =   0   ;
    const uint8     SSTYPE_UNKNOWN  =   0   ;
    const uint8     SSTYPE_INT8     =   1   ;
    const uint8     SSTYPE_UINT8    =   2   ;
    const uint8     SSTYPE_INT16    =   3   ;
    const uint8     SSTYPE_UINT16   =   4   ;
    const uint8     SSTYPE_INT32    =   5   ;
    const uint8     SSTYPE_UINT32   =   6   ;
    const uint8     SSTYPE_INT64    =   7   ;
    const uint8     SSTYPE_UINT64   =   8   ;
    const uint8     SSTYPE_BOOL     =   9   ;
    const uint8     SSTYPE_STRING   =   10  ;
    const uint8     SSTYPE_FLOAT    =   11  ;
    const uint8     SSTYPE_DOUBLE   =   12  ;
    const uint8     SSTYPE_RAW      =   13  ;
	const uint8		SSTYPE_T32		=	14	;		//等同于time_t，精度秒
	const uint8		SSTYPE_T64		=	15	;		//等同于time_t + 毫秒，精度毫秒
	const uint8		SSTYPE_BIT64	=	16	;
    const uint8     SSTYPE_MAX      =   16  ;

    static uint8 bits[8] = {1 , 2 , 4 , 8 , 16 , 32 , 64 , 128} ;

    inline bool get_bit_mask(uint8 val , int index)
    {
        if(index < 0 || index >= 8)
            return false ;

        return (val & bits[index]) != 0 ;
    }

    inline void set_bit_mask(uint8& val , int index , bool bv = false)
    {
        if(index < 0 || index >= 8)
            return ;

        if(((val & bits[index]) != 0) == bv)
            return ;

        val = val ^ bits[index];
    }

    uint32 get_type_size(uint8 type) ;
	const char * get_type_desc(uint8 type) ;

    bool float_equal(float f1 , float f2) ;
    bool double_equal(double d1 , double d2) ;

	const char * convert_int(const char * str , int size , int&value) ;

	 /*
		2011-07-07
		解析日期时间。支持格式为"YYYY-MM-DD HH:MM:SS.mmm"或者"YYYYMMDD HH:MM:SS.mmm" 或者"YYYY/MM/DD HH:MM:SS.mmm"
		如果日期有分隔符，那么可以不补充0
	 */
	bool convert_time(const char * str , int&year , int& month , int& day , int& hour , int& minute , int& second , int& msec) ;

	time_t make_time(int year , int month , int day , int hour = 0, int minute = 0, int second = 0) ;

	bool str2time(const char * str , time_t&  t , int& msec) ;

}

#endif  /** __SS_TYPE_H */
