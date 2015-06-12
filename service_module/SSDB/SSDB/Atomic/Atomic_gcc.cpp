
namespace ss{
	
	atomic_count::atomic_count( long v ) : value_( v ) {}

	long atomic_count::increment()
	{
		return __exchange_and_add( &value_, +1 ) + 1;
	}

	long atomic_count::decrement()
	{
		return __exchange_and_add( &value_, -1 ) - 1;
	}

	long atomic_count::value() const
	{
		return __exchange_and_add( &value_, 0 );
	}


	atomic_count::atomic_count(atomic_count const &);
	atomic_count & atomic_count::operator=(atomic_count const &);

}

