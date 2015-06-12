
#include "SSType.h"

namespace ss {

    static uint32 type_sizes[SSTYPE_MAX + 1] = {    0 , 
        1 , 1 , 2 , 2 ,
        4 , 4 , 8 , 8 ,
        1 , 0 , 4 , 8 ,
		0 , 4 , 8 , 8    
    } ;

    uint32 get_type_size(uint8 type)
    {
        if(type < SSTYPE_MIN || type > SSTYPE_MAX)
            return 0 ;

        return type_sizes[type] ;
    }

	static const char * type_descs[SSTYPE_MAX+1] = {
		"SSTYPE_UNKNOWN" , 
		"SSTYPE_INT8" , "SSTYPE_UINT8" , "SSTYPE_INT16" , "SSTYPE_UINT16" ,
		"SSTYPE_INT32" , "SSTYPE_UINT32" , "SSTYPE_INT64" , "SSTYPE_UINT64" ,
		"SSTYPE_BOOL" , "SSTYPE_STRING" , "SSTYPE_FLOAT" , "SSTYPE_DOUBLE" ,
		"SSTYPE_RAW" , "SSTYPE_T32" , "SSTYPE_T64" , "SSTYPE_BIT64"
	} ;

	const char * get_type_desc(uint8 type) 
	{
        if(type < SSTYPE_MIN || type > SSTYPE_MAX)
            return NULL ;

        return type_descs[type] ;	
	}

    bool float_equal(float f1 , float f2)
    {
        static float FIX_FLOAT_DELTA = 0.000001f ;

        float delta = f1 - f2 ;

        if(delta > -FIX_FLOAT_DELTA && delta < FIX_FLOAT_DELTA)
            return true ;
        else
            return false ;
    }

    bool double_equal(double d1 , double d2)
    {
        static double FIX_DOUBLE_DELTA = 0.0000000000000001 ;

        double delta = d1 - d2 ;

        if(delta > -FIX_DOUBLE_DELTA && delta < FIX_DOUBLE_DELTA)
            return true ;
        else
            return false ;
    
    }

	const char * convert_int(const char * str , int size , int&value)
	{
		bool finish = false ;
		char buf[32] ;
		int  index = 0 ;
		value = 0 ;

		if(str == NULL)
			return NULL ;

		char ch = 0 ;
		while(( ch = str[0]) != '\0' && finish == false)
		{
			if(ch >= '0' && ch <= '9')
			{
				buf[index++] = ch ;
				if(index >= size)
				{
					finish = true ;
				}
			}
			else
			{
				//保证是连续的几个数字作为一个值
				if(index != 0)
				{
					finish = true ;
				}

				if(finish)
				{
					buf[index] = '\0' ;
					value = atoi(buf) ;
				}
			}

			str++ ;
		
		}//end while ...

		return str ;	
	}

	bool convert_time(const char * str , int&year , int& month , int& day , int& hour , int& minute , int& second , int& msec)
	{
		year = month = day = hour = minute = second = msec = 0 ;
		if(str == NULL)
			return false ;

		const char * pchar = str ;
		int stage = 0 ;
		char ch = 0 , buffer[32] = {'\0'};
		int buflen = 0 ;
		do{
			ch = pchar[0] ;

			if(stage >= 0 && stage <= 2)
			{
				//日期，'-','/'以及最后的空格作为分隔符，如果没有分隔符，按长度分隔
				bool finish = false , seperate = false;
				if(ch >= '0' && ch <= '9')
				{
					buffer[buflen++] = ch ;
				}
				else
				{
					seperate = finish = true ;		//有分隔符，那么同时表示结束
				}

				if(finish == false)
				{
					if(stage == 0)
					{					
						if(buflen >= 5) 
						{
							finish = true ; 
							buflen = 4 ;
						}
					}
					else if(stage == 1 || stage == 2)
					{
						if(buflen >= 3) 
						{
							finish = true ; 
							buflen = 2 ;
						}
					}				
				}

				if(finish == true)
				{
					buffer[buflen] = '\0' ;
					switch(stage)
					{
					case 0 : year = atoi(buffer) ; break ;
					case 1 : month = atoi(buffer) ;  break ;
					case 2 : day = atoi(buffer) ; break ;
					default :break ;
					}

					buflen = 0 ;
					if(seperate == false)
					{
						buffer[buflen++] = ch ;
					}
					stage++ ;				
				}

			}
			else if(stage >= 3 && stage <= 6)
			{
				//时间，必须以":"分隔，中间可能没有0
				if(ch >= '0' && ch <= '9')
				{
					buffer[buflen++] = ch ;
				}
				else if(ch == ':' || ch == '.' || ch == '\0')
				{
					buffer[buflen] = '\0' ;
					switch(stage)
					{
					case 3 : hour = atoi(buffer) ; break ;
					case 4 : minute = atoi(buffer) ; break ;
					case 5 : second  = atoi(buffer) ; break ;
					case 6 : msec = atoi(buffer) ; break ;
					default : break ;
					}
				
					buflen = 0 ;
					stage++ ;
				}
			}

			pchar ++ ;	
		
		} while(ch != 0) ;

		return true ;	
	}

	time_t make_time(int year , int month , int day , int hour , int minute , int second )
	{
		struct tm tm ;
		memset(&tm , 0 , sizeof(tm)) ;

		tm.tm_year = year - 1900 ;
		tm.tm_mon = month - 1 ;
		tm.tm_mday = day ;

		tm.tm_hour = hour ;
		tm.tm_min = minute ;
		tm.tm_sec = second ;	

		return ::mktime(&tm) ;
	}

	bool str2time(const char * str , time_t&  t , int& msec) 
	{
		int year , month , day , hour , minute , second ;
		if(str == NULL || convert_time(str , year , month , day , hour , minute , second , msec) == false)
			return false ;

        int length = strlen(str);
        switch (length){
            case 4: //year(YYYY)
                t = year;
                break;
            case 8: //time(HH:MM:SS)
                //hour is year, min is month, sec is day
                t = year * 3600 + month * 60 + day;
                break;
            default://datetime or date
                t = make_time(year , month , day , hour , minute , second) ;
                break;
        }
		return true ;
	}
}

