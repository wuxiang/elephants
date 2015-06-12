
#include "Atomic/Atomic_gcc_x86.h"

namespace ss{

	atomic_count::atomic_count( long v ) : value_( static_cast< int >( v ) ) {}

	long atomic_count::increment()
	{
		return atomic_exchange_and_add( &value_, +1 ) + 1;
	}

	long atomic_count::decrement()
	{
		return atomic_exchange_and_add( &value_, -1 ) - 1;
	}

	long atomic_count::value() const
	{
		return atomic_exchange_and_add( &value_, 0 );
	}


	atomic_count::atomic_count(atomic_count const &)
        {

        }
#if 0
	atomic_count & atomic_count::operator=(atomic_count const &)
        {

        }
#endif
	int atomic_count::atomic_exchange_and_add( int * pw, int dv )
	{

		int r;

		__asm__ __volatile__
		(
			"lock\n\t"
			"xadd %1, %0":
			"+m"( *pw ), "=r"( r ): // outputs (%0, %1)
			"1"( dv ): // inputs (%2 == %1)
			"memory", "cc" // clobbers
		);

		return r;
	}
}
