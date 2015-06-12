
#ifndef __SS_ATOMIC_H
#define __SS_ATOMIC_H 1


#if defined( __GNUC__ ) && ( defined( __i386__ ) || defined( __x86_64__ ) )
	#  include <Atomic/Atomic_gcc_x86.h>
#else
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__) || defined(MINGW) || defined(MINGW32)
		#include <Atomic/Atomic_win32.h>
/*
	#else
		#if defined(__GLIBCPP__) || defined(__GLIBCXX__)
			#include <Atomic/Atomic_gcc.h>
		#endif
*/
	#endif
#endif

#endif /** __SS_ATOMIC_H */

