
#ifndef ATOMIC_WIN32_H
#define ATOMIC_WIN32_H 1

namespace ss{
	
	class atomic_count
	{
	public:
		explicit atomic_count( long v = 0) ;
		long increment() ;
		long decrement() ;
		long value() const ;	
	private:
	
	    atomic_count( atomic_count const & );
	    atomic_count & operator=( atomic_count const & );
	
	    long volatile value_;
	};
	
}

#endif /** ATOMIC_WIN32_H */

