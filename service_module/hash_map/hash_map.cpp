
#include "hash_map.h"


msgbus::uint32_t _hash_key(msgbus::int32_t key) 
{
    return (msgbus::uint32_t)key ;
}

msgbus::uint32_t _hash_key(msgbus::int64_t key) 
{
    return (msgbus::uint32_t)key ;
}


msgbus::uint32_t _hash_key(msgbus::uint32_t key)
{
    return (msgbus::uint32_t)key ;
}

msgbus::uint32_t _hash_key(msgbus::uint64_t key)
{
    return (msgbus::uint32_t)key ;
}

msgbus::uint32_t _hash_key(const char * key)
{
    if(key == NULL)
        return 0 ;

    msgbus::uint32_t hash = 0 ;
    const msgbus::uint8_t * p = (const msgbus::uint8_t *) key ;

    msgbus::uint8_t ch ;
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



