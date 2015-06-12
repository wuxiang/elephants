
#include "Atomic_win32.h"
#include <windows.h>


namespace ss{

	atomic_count::atomic_count( long v )
	{
		value_ = v ;
	}

	long atomic_count::increment()
	{
		return InterlockedIncrement( &value_ );
	}

	long atomic_count::decrement()
	{
		return InterlockedDecrement( &value_ );
	}

	long atomic_count::value() const
	{
		return static_cast<long const volatile &>( value_ );
	}	
}


