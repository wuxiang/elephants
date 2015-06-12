
#ifndef __ATOMIC_GCC_X86_H
#define __ATOMIC_GCC_X86_H 1

namespace ss{
	
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
	
	    mutable int value_;
	
	private:
	
	    static int atomic_exchange_and_add( int * pw, int dv ) ;
	};
}

#endif /** __ATOMIC_GCC_X86_H */
