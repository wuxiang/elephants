
#ifndef __SS_STRING_H
#define __SS_STRING_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

namespace ss {

int hex_sprintf(char * strbuf , char val) ;

const char CHAR_SPACE	=		32 ;	// ' '
const char CHAR_GREATER	=	    62 ;    // '>'
const char CHAR_DOT		=	    46 ;    // '.'

const char * string_find_first_of(const char * str , const char * begin , const char separator) ;

int string_split(const std::string& str , std::string *str_array , int count , const char separator = CHAR_SPACE) ;

int string_split(const std::string& str , std::vector<std::string>& str_vector , const char separator = CHAR_SPACE) ;

int string_split(const char * str , std::string *str_array , int count , const char separator = CHAR_SPACE) ;

int string_split(const char * str , std::vector<std::string>& str_vector , const char separator = CHAR_SPACE) ;

std::string string_error(int errcode) ;

int to_uppercase(const char *str , char *uc) ; 

int string_scan(const char *str , int len) ; 

}

#endif  /** __SS_STRING_H */
