
#include "sdbus/hash_map.h"

namespace sdbus{

uint32_t _hash_key(int32_t key) 
{
    return (uint32_t)key ;
}

uint32_t _hash_key(int64_t key) 
{
    return (uint32_t)key ;
}


uint32_t _hash_key(uint32_t key)
{
    return (uint32_t)key ;
}

uint32_t _hash_key(uint64_t key)
{
    return (uint32_t)key ;
}

uint32_t _hash_key(const char * key)
{
    if(key == NULL)
        return 0 ;

    uint32_t hash = 0 ;
    const uint8_t * p = (const uint8_t *) key ;

    uint8_t ch ;
    while((ch = *p) != 0)
    {
        hash = (hash << 5) + hash + ch ;
        ++p ;
    }

    return hash ;
}

bool _hash_compare(const char * k1 , const char * k2) 
{
    return (::strcmp(k1 , k2) == 0) ;
}


}

