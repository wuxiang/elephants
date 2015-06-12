
#include "SSSafeObject.h"


namespace ss{

	safe_object_ref::safe_object_ref()
	{
		object_ = NULL ;
		auto_free_ = true ;
	}

	safe_object_ref::safe_object_ref(safe_object * object , bool auto_free)
	{
		object_ = NULL ;
		attach(object , auto_free) ;
	}

	safe_object_ref::safe_object_ref(const safe_object_ref &ref)
	{
		object_ = NULL ;
		attach(ref.object_ , ref.auto_free_) ;
	}

	safe_object_ref& safe_object_ref::operator=(const safe_object_ref& ref)
	{
		attach(ref.object_ , ref.auto_free_) ;
		return (*this) ;
	}

	safe_object_ref::~safe_object_ref()
	{
		detach() ;
	}

	safe_object * safe_object_ref::get() const
	{
		return object_ ;
	}

	bool safe_object_ref::get_auto_free() const
	{
   		return auto_free_ ;	
	}

	void safe_object_ref::attach(safe_object *object , bool auto_free) 
	{
		if(object != NULL)
		{
			object->increment() ;
		}

		detach() ;
		object_ = object ;
		auto_free_ = auto_free ;
	}

	void safe_object_ref::detach() 
	{
		if(object_ != NULL)
		{
			long val = object_->decrement() ;
			if(val == 0 && auto_free_ == true)
			{
				delete object_ ;
			}
			object_ = NULL ;	
		}
	}

	safe_object * safe_object_ref::operator->() const
	{
		return object_ ;
	}

	bool safe_object_ref::check_valid() const
	{
		return (object_ != NULL) ;
	}

	bool safe_object_ref::operator==(const safe_object_ref& ref)
	{
		return (object_ == ref.object_) ;
	}

}
