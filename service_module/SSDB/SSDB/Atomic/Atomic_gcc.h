
#ifndef __ATOMIC_GCC_H
#define __ATOMIC_GCC_H 1


#if __GNUC__ * 100 + __GNUC_MINOR__ >= 402
# include <ext/atomicity.h> 
#else 
# include <bits/atomicity.h>
#endif

namespace ss{
#if defined(__GLIBCXX__) // g++ 3.4+

using __gnu_cxx::__atomic_add;
using __gnu_cxx::__exchange_and_add;

#endif

	class atomic_count
	{
	public:
	
	    explicit atomic_count( long v = 0) ;
	
	    long increment() ;
	
	    long decrement() ;
	
	    long value() const ;
	
	private:
	
	    atomic_count(atomic_count const &);
	    atomic_count & operator=(atomic_count const &);
	
	    mutable _Atomic_word value_;
	};
}

#endif  /** __ATOMIC_GCC_H */

