
#ifndef __SS_SAFE_OBJECT_H
#define __SS_SAFE_OBJECT_H 1

#include <stdio.h>
#include <stdlib.h>

#include "SSAtomic.h"

namespace ss{

	class safe_object : public atomic_count {
	public:
		virtual ~safe_object() {}
	} ;

	class safe_object_ref {
	private:
		safe_object * object_ ;
		bool auto_free_ ;
	public:
		safe_object_ref() ;
		safe_object_ref(safe_object * object , bool auto_free = true) ;
		safe_object_ref(const safe_object_ref &ref) ;
		virtual ~safe_object_ref() ;

		safe_object * get() const;

		bool get_auto_free() const ;

		void attach(safe_object *object , bool auto_free = true) ;
		void detach() ;

		safe_object_ref& operator=(const safe_object_ref& ref) ;
		bool operator==(const safe_object_ref& ref) ;

		safe_object * operator->() const ;

		bool check_valid() const ;
	} ;
}


#endif /**__SS_SAFE_OBJECT_H */

