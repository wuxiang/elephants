
#include "SSString.h"

#if defined(WIN32) || defined(_WINDOWS) || defined(MINGW32)
#include <windows.h> 
#endif


namespace ss {

	/*
		2011-03-01
		MINGW居然不支持printf("%02hhx" , 2)的格式，只能自己实现	
	*/
	static char hex_map[16] = {
		'0'	,	'1'	,	'2'	,	'3'	,
		'4'	,	'5'	,	'6'	,	'7'	,
		'8'	,	'9'	,	'A'	,	'B'	,
		'C'	,	'D'	,	'E'	,	'F'
	} ;

	int hex_sprintf(char * strbuf , char val)
	{
		strbuf[0] = hex_map[(val >> 4) & 0XF] ;
		strbuf[1] = hex_map[val & 0XF] ;
		strbuf[2] = '\0' ;
		return 2 ;
	}


	const char * string_find_first_of(const char * str , const char * begin , const char separator)
	{
		const char * end = begin ;

		if(begin == NULL)
			return NULL ;

		while(true)
		{
			if(((*end) == '\0') || ((*end) == separator))
			{
				break ;
			}

			end++ ;
		}

		return end ;
	}

	int string_split(const std::string& str , std::string *str_array , int count , const char separator)
	{
		return string_split(str.c_str() , str_array , count ,separator) ;
	}

	int string_split(const std::string& str , std::vector<std::string>& str_vector , const char separator)
	{
		return string_split(str.c_str() , str_vector , separator) ;
	}

	int string_split(const char * str , std::string *str_array , int count , const char separator)
	{
		if(str_array == NULL || count <= 0 || separator == '\0')
			return 0 ;

		const char * begin , * end;
		end = begin = str ;
		int index = 0 ;
		
		while(index < count)		
		{
			end = string_find_first_of(str , begin , separator) ;
			if(begin != end)
			{
				str_array[index++].assign(begin , (std::string::size_type)(end - begin)) ;
				begin = end ;
			}

			if((*begin) != '\0')
			{
				begin++ ;
			}
			else
				break ;			
		}

		return index ;
	}

	int string_split(const char * str , std::vector<std::string>& str_vector , const char separator)
	{
		if(separator == '\0')
			return 0 ;

		const char * begin , *end;
		end = begin = str ;
		int index = str_vector.size() , capacity = str_vector.capacity() , step = capacity ;
		if(step <= 0)
			step = 8 ;
		
		while(true)		
		{
			end = string_find_first_of(str , begin , separator) ;
			if(begin != end)
			{
				/*
					保证不会下标溢出	
				*/
				if(index >= capacity)
				{
					capacity += step ;
					step = capacity >> 2 ;
					if(step < 8)
						step = 8 ;

					str_vector.resize(capacity) ;
				}
				str_vector[index++].assign(begin , (std::string::size_type)(end - begin)) ;

				begin = end ;
			}

			if((*begin) != '\0')
			{
				begin++ ;
			}
			else
				break ;			
		}

		return index ;	
	}

	std::string string_error(int errcode)
	{
		char errmsg[1024] = {'\0'};

#if defined(WIN32) || defined(_WINDOWS) || defined(MINGW32)
		FormatMessageA(
			 FORMAT_MESSAGE_FROM_SYSTEM |
			 FORMAT_MESSAGE_IGNORE_INSERTS,
			 NULL,
			 errcode ,
			 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			 errmsg ,
			 0, NULL );


#else
		if(strerror_r(errcode , errmsg , 1024) != 0)
		{
			errmsg[0] = '\0' ;
		}
#endif
		return errmsg ;
	}


	int to_uppercase(const char *str , char *uc)
	{
		int len = 0 ;
		uc[0]  = '\0' ;

		if(str == NULL)
			return 0 ;

		for(const char *pchar = str ; pchar[0] != '\0' ; pchar++ , len++)
		{
			if(pchar[0] >= 'a' && pchar[0] <= 'z')
			{
				uc[len] = pchar[0] - 'a' + 'A' ;
			}
			else
			{
				uc[len] = pchar[0] ;
			}
		}

		uc[len] = '\0' ;

		return len ;
	}

	int string_scan(const char *str , int len) 
	{
		if(str == NULL || len <= 0)
			return 0;

		const char *pchar = str ;
		int idx = 0 ;
		for(; (idx < len) && pchar[idx] != '\0' ; idx++) ;

		if(idx == len)
		{
			//没有找到'\0'
			return 0 ;
		}

		idx++ ;
		return idx ;
	}

}


