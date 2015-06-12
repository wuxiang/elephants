
#include "SSTuple.h"

namespace ss {


	tuple::header::header()
    {
        //
		size_ = 0 ;
    }

    tuple::header::header(const tuple::header& h)
    {
        names_ = h.names_ ;
        defs_ = h.defs_ ;
		size_ = h.size() ;
    }

    int tuple::header::add(const field_def& def)
    {
        if(def == field_def::FIELD_NULL)
            return -1 ;

		int index = def.index ;
		if(index < 0)
		{
			defs_.push_back(def) ;
			index = (int)defs_.size() - 1 ;
		}
		else
		{
			//指定下标
			if(index >= (int)defs_.size())
			{
				defs_.resize(index + 1) ;
			}

			defs_[index] = def ;		
		}

		int offset = def.offset ;
		if(offset < 0)
		{
			offset = index * (int)sizeof(field) ;
		}

		defs_[index].index = index ;
		defs_[index].offset = offset ;

		if(offset + (int)sizeof(field) > size_)
			size_ = offset + (int)sizeof(field) ;


		/*
			允许重名，但是如果重名，就不能通过名字来访问
		*/
		std::map<std::string , int>::iterator iter = names_.find(def.name) ;
		if(iter == names_.end())
			names_[def.name] = index ;


		return index ;
    }

    int tuple::header::add(const std::string& name ,const std::string &table, uint8 type , int idx , int of)
    {
        return add(field_def(name , table , type , idx , of)) ;
    }
    
    const field_def& tuple::header::fields(int index) const
    {
        if(index < 0 || index >= (int)defs_.size())
			return field_def::FIELD_NULL ;
        else
            return defs_[index] ;
    }

    void tuple::header::setDecimals(const std::string &name, unsigned int decimals)
    {
        const_cast<field_def &>(fields(name)).decimals = decimals;
    }

    unsigned int tuple::header::getDecimals(const std::string &name) const
    {
        return fields(name).decimals;
    }

    void tuple::header::setMaxLength(const std::string &name, unsigned long max_length)
    {
        const_cast<field_def &>(fields(name)).max_length = max_length;
    }

    unsigned long tuple::header::getMaxLength(const std::string &name) const
    {
        return fields(name).max_length;
    }

    void tuple::header::setLength(const std::string &name, unsigned long length)
    {
        const_cast<field_def &>(fields(name)).length = length;
    }

    unsigned long tuple::header::getLength(const std::string &name) const
    {
        return fields(name).length;
    }

    const field_def& tuple::header::fields(const std::string& name) const
    {
		std::map<std::string , int>::const_iterator iter = names_.find(name) ;
        if(iter == names_.end())
            return field_def::FIELD_NULL ;

        int index = iter->second ;
        if(index < 0 || index >= (int)defs_.size())
            return field_def::FIELD_NULL ;

        return defs_[index] ;
    }

	int tuple::header::find(const std::string& name) const
	{
		std::map<std::string , int>::const_iterator iter = names_.find(name) ;
		if(iter == names_.end())
			return -1 ;
		return iter->second ;
	}

	void tuple::header::clear()
	{
		defs_.clear() ;
		names_.clear() ;
		primary_.clear();
		size_ = 0 ;	
	}


	tuple::tuple()
	{
		header_ = NULL ;
		fields_ = NULL ;
		field_count_ = 0 ;
	}

	tuple::tuple(const tuple& t)
	{
		header_ = t.header_ ;
		fields_ = t.fields_ ;
		field_count_ = t.field_count_ ;
	}

	tuple::~tuple()
	{
		header_ = NULL ;
		fields_ = NULL ;
		field_count_ = 0 ;	
	}

	const tuple::header * tuple::get_header() const
	{
		return header_ ;
	}

	void tuple::init(tuple::header * h , field * fs , int fc)
	{
		header_ = h ; 
		fields_ = fs ;
		field_count_ = fc ;
	}

	const field& tuple::fields(int index) const
	{
		if(fields_ == NULL || index < 0 || index >= field_count_)
			return field::FIELD_NULL ;

		return fields_[index] ;
	}

	const field& tuple::fields(const std::string& name) const
	{
		if(header_ == NULL)
			return field::FIELD_NULL ;

		int index = header_->find(name) ;
		if(index < 0 || index >= field_count_)
			return field::FIELD_NULL ;

		return fields_[index] ;
	}

	int tuple::find(const std::string& name) const
	{
		if(header_ == NULL)
			return -1 ;

		return header_->find(name) ;	
	}

	bool tuple::update_field_int8(const std::string& name , int8 value)
	{
		field * f = check_field(name , SSTYPE_INT8) ;
		if(f == NULL)
			return false ;

		fieldi8 *i8 = (fieldi8 *)f ;
		*i8 = value ;
		return true ;
	}

	bool tuple::update_field_uint8(const std::string& name , uint8 value)
	{
		field * f = check_field(name , SSTYPE_UINT8) ;
		if(f == NULL)
			return false ;

		fieldu8 *u8 = (fieldu8 *)f ;
		*u8 = value ;
		return true ;
	}

	bool tuple::update_field_int16(const std::string& name , int16 value)
	{
		field * f = check_field(name , SSTYPE_INT16) ;
		if(f == NULL)
			return false ;

		fieldi16 *i16 = (fieldi16 *)f ;
		*i16 = value ;
		return true ;
	}

	bool tuple::update_field_uint16(const std::string& name , uint16 value)
	{
		field * f = check_field(name , SSTYPE_UINT16) ;
		if(f == NULL)
			return false ;

		fieldu16 *u16 = (fieldu16 *)f ;
		*u16 = value ;
		return true ;
	}

	bool tuple::update_field_int32(const std::string& name , int32 value)
	{
		field * f = check_field(name , SSTYPE_INT32) ;
		if(f == NULL)
			return false ;

		fieldi32 *i32 = (fieldi32 *)f ;
		*i32 = value ;
		return true ;
	}

	bool tuple::update_field_uint32(const std::string& name , uint32 value)
	{
		field * f = check_field(name , SSTYPE_UINT32) ;
		if(f == NULL)
			return false ;

		fieldu32 *u32 = (fieldu32 *)f ;
		*u32 = value ;
		return true ;
	}

	bool tuple::update_field_int64(const std::string& name , int64 value)
	{
		field * f = check_field(name , SSTYPE_INT64) ;
		if(f == NULL)
			return false ;

		fieldi64 *i64 = (fieldi64 *)f ;
		*i64 = value ;
		return true ;
	}

	bool tuple::update_field_uint64(const std::string& name , uint64 value)
	{
		field * f = check_field(name , SSTYPE_UINT64) ;
		if(f == NULL)
			return false ;

		fieldu64 *u64 = (fieldu64 *)f ;
		*u64 = value ;
		return true ;
	}


	bool tuple::update_field_float(const std::string& name , float value)
	{
		field * f = check_field(name , SSTYPE_FLOAT) ;
		if(f == NULL)
			return false ;

		fieldf32 *f32 = (fieldf32 *)f ;
		*f32 = value ;
		return true ;
	}

	bool tuple::update_field_double(const std::string& name , double value)
	{
		field * f = check_field(name , SSTYPE_DOUBLE) ;
		if(f == NULL)
			return false ;

		fieldf64 *f64 = (fieldf64 *)f ;
		*f64 = value ;
		return true ;
	}

	bool tuple::update_field_bool(const std::string& name , bool value)
	{	
		field * f = check_field(name , SSTYPE_BOOL) ;
		if(f == NULL)
			return false ;

		fieldbv *bv = (fieldbv *)f ;
		*bv = value ;
		return true ;
	}

	bool tuple::update_field_string(const std::string& name , const char * value)
	{
		field * f = check_field(name , SSTYPE_STRING) ;
		if(f == NULL)
			return false ;

		fieldstr *str = (fieldstr *)f ;
		*str = value ;
		return true ;
	}

    bool tuple::update_field_raw(const std::string& name , const char * value , int len)
	{
		field * f = check_field(name , SSTYPE_RAW) ;
		if(f == NULL)
			return false ;

		fieldraw *raw = (fieldraw *)f ;
		raw->copy(value , (uint32)len) ;
		return true ;
	}

	bool tuple::get_field_int8(int index , int8 & value) const
	{
		field * f = check_field(index , SSTYPE_INT8) ;
		if(f == NULL)
			return false ;

		fieldi8 *i8 = (fieldi8 *)f ;
		value = *i8 ;
		return true ;
	}

	bool tuple::get_field_uint8(int index , uint8 & value) const
	{
		field * f = check_field(index , SSTYPE_UINT8) ;
		if(f == NULL)
			return false ;

		fieldu8 *u8 = (fieldu8 *)f ;
		value = *u8 ;
		return true ;
	}

	bool tuple::get_field_int16(int index , int16 & value) const
	{
		field * f = check_field(index , SSTYPE_INT16) ;
		if(f == NULL)
			return false ;

		fieldi16 *i16 = (fieldi16 *)f ;
		value = *i16 ;
		return true ;
	}

	bool tuple::get_field_uint16(int index , uint16 & value) const
	{
		field * f = check_field(index , SSTYPE_UINT16) ;
		if(f == NULL)
			return false ;

		fieldu16 *u16 = (fieldu16 *)f ;
		value = *u16 ;
		return true ;
	}


	bool tuple::get_field_int32(int index , int32 & value) const
	{
		field * f = check_field(index , SSTYPE_INT32) ;
		if(f == NULL)
			return false ;

		fieldi32 *i32 = (fieldi32 *)f ;
		value = *i32 ;
		return true ;
	}

	bool tuple::get_field_uint32(int index , uint32 & value) const
	{
		field * f = check_field(index , SSTYPE_UINT32) ;
		if(f == NULL)
			return false ;

		fieldu32 *u32 = (fieldu32 *)f ;
		value = *u32 ;
		return true ;
	}

	bool tuple::get_field_int64(int index , int64 & value) const
	{
		field * f = check_field(index , SSTYPE_INT64) ;
		if(f == NULL)
			return false ;

		fieldi64 *i64 = (fieldi64 *)f ;
		value = *i64 ;
		return true ;

	}

	bool tuple::get_field_uint64(int index , uint64 & value) const
	{
		field * f = check_field(index , SSTYPE_UINT64) ;
		if(f == NULL)
			return false ;

		fieldu64 *u64 = (fieldu64 *)f ;
		value = *u64 ;
		return true ;
	}


	bool tuple::get_field_float(int index , float& value) const
	{
		field * f = check_field(index , SSTYPE_FLOAT) ;
		if(f == NULL)
			return false ;

		fieldf32 *f32 = (fieldf32 *)f ;
		value = *f32 ;
		return true ;
	}

	bool tuple::get_field_double(int index , double& value) const
	{
		field * f = check_field(index , SSTYPE_DOUBLE) ;
		if(f == NULL)
			return false ;

		fieldf64 *f64 = (fieldf64 *)f ;
		value = *f64 ;
		return true ;
	}

	bool tuple::get_field_bool(int index , bool& value) const
	{
		field * f = check_field(index , SSTYPE_BOOL) ;
		if(f == NULL)
			return false ;

		fieldbv *bv = (fieldbv *)f ;
		value = *bv ;
		return true ;
	}

	bool tuple::get_field_string(int index , const char* & value , int& len) const
	{
		field * f = check_field(index , SSTYPE_STRING) ;
		if(f == NULL)
			return false ;

		fieldstr *str = (fieldstr *)f ;
		value = str->c_str() ;
		len = str->length() ;
		return true ;
	}

    bool tuple::get_field_raw(int index , const char* & value , int& len) const
	{
		field * f = check_field(index , SSTYPE_RAW) ;
		if(f == NULL)
			return false ;

		fieldraw *raw = (fieldraw *)f ;
		const void * buf = *raw ;
		value = (const char *)buf ;
		len = raw->length() ;
		return true ;
	}


	field * tuple::check_field(int index , uint8 type) const
	{
		if(index < 0 || index >= field_count_)
			return NULL ;

		if(fields_ == NULL)
			return NULL ;

		field * f = fields_ + index ;
		if(f->type() != type)
			return NULL ;
		else
			return f ;
	}

	field * tuple::check_field(const std::string& name , uint8 type) const
	{
		return check_field(find(name) , type) ;
	}

}

