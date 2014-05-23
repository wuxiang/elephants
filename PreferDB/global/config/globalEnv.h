#ifndef ALLOCATE_CONFIG_H_
#define ALLOCATE_CONFIG_H_

namespace Elephants
{

#if defined(WIN32) || defined(_WINDOWS) || defined(_WIN32) || defined(_WIN64)

    typedef unsigned char       uint8_t;
    typedef signed char         int8_t;
    typedef unsigned short      uint16_t;
    typedef short               int16_t;
    typedef unsigned int        uint32_t;
    typedef int                 int32_t;
    typedef unsigned __int64    uint64_t;
    typedef __int64             int64_t;

#else
#include <stdint.h>
    typedef ::uint8_t uint8_t ;
    typedef ::int8_t  int8_t ;
    typedef ::uint16_t uint16_t ;
    typedef ::int16_t  int16_t ;
    typedef ::uint32_t uint32_t ;
    typedef ::int32_t  int32_t ;
    typedef ::uint64_t uint64_t ;
    typedef ::int64_t  int64_t ; 
#endif

}




#endif

