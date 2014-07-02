//
//  here it's all hash algorithm for string,
//  and BKDRHash is best all of hash function below,
//  APHash is less than BKDRHash, but better than others
// 
//  below it's effective table
//Hash����	 ����1	 ����2	 ����3	 ����4	 ����1�÷�	 ����2�÷�	 ����3�÷�	 ����4�÷�	 ƽ����
//BKDRHash	  2	      0	     4774	 481	 	96.55	 	100	 		90.95	 	82.05	 92.64
//APHash	  2	      3	     4754	 493	 	96.55	 	88.46	 	100	 		51.28	 86.28
//DJBHash	  2	      2	     4975	 474	 	96.55	 	92.31	 	0	 		100	 	 83.43
//JSHash	  1	      4	     4761	 506	 	100	 		84.62	 	96.83	 	17.95	 81.94
//RSHash	  1	      0	     4861	 505	 	100	 		100	 		51.58	 	20.51	 75.96
//SDBMHash	  3	      2	     4849	 504	 	93.1	 	92.31	 	57.01	 	23.08	 72.41
//PJWHash	 30	     26	     4878	 513	 	0	 		0	 		43.89	 	0	 	 21.95
//ELFHash	 30	     26	     4878	 513	 	0	 		0	 		43.89	 	0	 	 21.95

#ifndef STR_HASH_HPP
#define STR_HASH_HPP

inline unsigned int SDBMHash(char* str)
{
	unsigned int hash = 0;
	 
	while (*str)
	{
	// equivalent to: hash = 65599*hash + (*str++);
	hash = (*str++) + (hash << 6) + (hash << 16) - hash;
	}
	 
	return (hash & 0x7FFFFFFF);
}

// RS Hash Function
inline unsigned int RSHash(char *str)
{
    unsigned int b = 378551;
    unsigned int a = 63689;
    unsigned int hash = 0;
 
    while (*str)
    {
        hash = hash * a + (*str++);
        a *= b;
    }
 
    return (hash & 0x7FFFFFFF);
}
 
// JS Hash Function
inline unsigned int JSHash(char *str)
{
    unsigned int hash = 1315423911;
 
    while (*str)
    {
        hash ^= ((hash << 5) + (*str++) + (hash >> 2));
    }
 
    return (hash & 0x7FFFFFFF);
}
 
// P. J. Weinberger Hash Function
inline unsigned int PJWHash(char *str)
{
    unsigned int BitsInUnignedInt = (unsigned int)(sizeof(unsigned int) * 8);
    unsigned int ThreeQuarters    = (unsigned int)((BitsInUnignedInt  * 3) / 4);
    unsigned int OneEighth        = (unsigned int)(BitsInUnignedInt / 8);
    unsigned int HighBits         = (unsigned int)(0xFFFFFFFF) << (BitsInUnignedInt - OneEighth);
    unsigned int hash             = 0;
    unsigned int test             = 0;
 
    while (*str)
    {
        hash = (hash << OneEighth) + (*str++);
        if ((test = hash & HighBits) != 0)
        {
            hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
        }
    }
 
    return (hash & 0x7FFFFFFF);
}
 
// ELF Hash Function
inline unsigned int ELFHash(char *str)
{
    unsigned int hash = 0;
    unsigned int x    = 0;
 
    while (*str)
    {
        hash = (hash << 4) + (*str++);
        if ((x = hash & 0xF0000000L) != 0)
        {
            hash ^= (x >> 24);
            hash &= ~x;
        }
    }
 
    return (hash & 0x7FFFFFFF);
}
 
// BKDR Hash Function
inline unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
 
    while (*str)
    {
        hash = hash * seed + (*str++);
    }
 
    return (hash & 0x7FFFFFFF);
}
 
// DJB Hash Function
inline unsigned int DJBHash(char *str)
{
    unsigned int hash = 5381;
 
    while (*str)
    {
        hash += (hash << 5) + (*str++);
    }
 
    return (hash & 0x7FFFFFFF);
}
 
// AP Hash Function
inline unsigned int APHash(char *str)
{
    unsigned int hash = 0;
    int i;
 
    for (i=0; *str; i++)
    {
        if ((i & 1) == 0)
        {
            hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
        }
    }
 
    return (hash & 0x7FFFFFFF);
}

#endif

