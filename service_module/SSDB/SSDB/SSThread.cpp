
#include "SSThread.h"


#if defined(WIN32) || defined(_WINDOWS)
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace ss {

	uint32 current_thread_id()
	{
	#if defined(WIN32) || defined(_WINDOWS)
			return (uint32) ::GetCurrentThreadId() ;
	#else
			return (uint32) pthread_self() ;
	#endif			
	}

}

