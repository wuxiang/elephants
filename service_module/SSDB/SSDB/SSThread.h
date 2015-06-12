
#ifndef __SS_THREAD_H
#define __SS_THREAD_H 1

/*
	2012-06-15
	导入boost的线程库，为以后替换提供可能
*/

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/locks.hpp>

#include "SSType.h"

namespace ss {

	using namespace boost ;

	uint32 current_thread_id() ;

}

#endif  /** __SS_THREAD_H */
