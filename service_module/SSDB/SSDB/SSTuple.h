
#ifndef __SS_TUPLE_H
#define __SS_TUPLE_H 1

#include "SSType.h"
#include "SSField.h"
#include <string>
#include <map>
#include <vector>

namespace ss{

	/*
		2012-06-15
		tuple被设计为field的容器，主要是为了提供一个访问接口。
		对于field容器来说，有2种情况需要区分，
		一个是由固定结构的，可以指定同一个头部。
		一个是没有固定结构，每一次都会有一个新的结构定义。

		因此，我们在这里只提供update和get接口
	*/
	class db_result;
	class tuple {
	public:
		class header{
		private:
			std::map<std::string , int> names_ ;		//特里树占的空间有点大，先用这个
			//从const char * 自动转换成const std::string，需要做内存拷贝，是个很讨厌的特性。
			std::vector<field_def> defs_ ;
			std::vector<std::string> primary_;
			int size_ ;
		public:
			header() ;
			header(const header& h) ;

			int add(const field_def& def) ;
			int add(const std::string& name ,const std::string &table, uint8 type , int idx = -1 , int of = -1) ;
			void add_primary(const std::string &primary){primary_.push_back(primary);}
			const std::string &get_primary(int count) const {return primary_[count];}

			void setDecimals(const std::string &name, unsigned int decimals);
			unsigned int getDecimals(const std::string &name) const;

			void setMaxLength(const std::string &name, unsigned long max_length);
			unsigned long getMaxLength(const std::string &name) const;

			void setLength(const std::string &name, unsigned long length);
			unsigned long getLength(const std::string &name) const;

			const field_def& fields(int index) const ;
			const field_def& fields(const std::string& name) const;
			int find(const std::string& name) const ;

			inline int size() const {return size_ ;}
			inline void size(int s) {size_ = s ;}
			inline bool empty() const {return defs_.empty() ;}
			void clear() ;

			inline int field_count() const {return (int) defs_.size() ;}
            inline int primary_count() const {return primary_.size();}
            friend class ss::db_result;
        } ;
	
		tuple() ;
		tuple(const tuple& t) ;
		virtual~tuple() ;

		const header * get_header() const ;
		void init(header * h , field * fs , int fc) ;

		const field& fields(int index) const;
		const field& fields(const std::string& name) const;

		inline int field_count() const {return field_count_ ;}
		int find(const std::string& name) const;

		/*
			更新
		*/
		bool update_field_int8(const std::string& name , int8 value) ;
		bool update_field_uint8(const std::string& name , uint8 value) ;
   		bool update_field_int16(const std::string& name , int16 value) ;
		bool update_field_uint16(const std::string& name , uint16 value) ;
	
   		bool update_field_int32(const std::string& name , int32 value) ;
		bool update_field_uint32(const std::string& name , uint32 value) ;
   		bool update_field_int64(const std::string& name , int64 value) ;
		bool update_field_uint64(const std::string& name , uint64 value) ;

   		bool update_field_float(const std::string& name , float value) ;
		bool update_field_double(const std::string& name , double value) ;
   		bool update_field_bool(const std::string& name , bool value) ;
		bool update_field_string(const std::string& name , const char * value) ;
        bool update_field_raw(const std::string& name , const char * value , int len) ;


		/*
			提取
		*/ 
		inline bool get_field_int8(const std::string& name , int8 & value) const
		{
			return get_field_int8(find(name) , value) ;
		}

		inline bool get_field_uint8(const std::string& name , uint8 & value) const
		{
			return get_field_uint8(find(name) , value) ;
		}

		inline bool get_field_int16(const std::string& name , int16 & value) const
		{
			return get_field_int16(find(name) , value) ;
		}

		inline bool get_field_uint16(const std::string& name , uint16 & value) const
		{
			return get_field_uint16(find(name) , value) ;
		}

		inline bool get_field_int32(const std::string& name , int32 & value) const
		{
			return get_field_int32(find(name) , value) ;
		}

		inline bool get_field_uint32(const std::string& name , uint32 & value) const
		{
			return get_field_uint32(find(name) , value) ;
		}

		inline bool get_field_int64(const std::string& name , int64 & value) const
		{
			return get_field_int64(find(name) , value) ;
		}

		inline bool get_field_uint64(const std::string& name , uint64 & value) const
		{
			return get_field_uint64(find(name) , value) ;
		}

		inline bool get_field_float(const std::string& name , float& value) const
		{
			return get_field_float(find(name) , value) ;
		}

		inline bool get_field_double(const std::string& name , double& value) const
		{
			return get_field_double(find(name) , value) ;
		}

		inline bool get_field_bool(const std::string& name , bool& value) const
		{
			return get_field_bool(find(name) , value) ;
		}

		inline bool get_field_string(const std::string& name , const char* & value , int& len) const
		{
			return get_field_string(find(name) , value , len) ;
		}

		inline bool get_field_raw(const std::string& name , const char* & value , int& len) const
		{
			return get_field_raw(find(name) , value , len) ;
		}

   		bool get_field_int8(int index , int8 & value) const;
		bool get_field_uint8(int index , uint8 & value) const;
   		bool get_field_int16(int index , int16 & value) const;
		bool get_field_uint16(int index , uint16 & value) const;
	
   		bool get_field_int32(int index , int32 & value) const;
		bool get_field_uint32(int index , uint32 & value) const;
   		bool get_field_int64(int index , int64 & value) const;
		bool get_field_uint64(int index , uint64 & value) const;

   		bool get_field_float(int index , float & value) const;
		bool get_field_double(int index , double & value) const;
   		bool get_field_bool(int index , bool & value) const;
		bool get_field_string(int index , const char* & value , int& len) const;
        bool get_field_raw(int index , const char* & value , int& len) const;
	
	protected:
		header	*header_ ;					//头部，字段描述
		field	*fields_ ;					//字段列表，包含值
		int		field_count_ ;				//字段数

		field * check_field(int index , uint8 type) const;
		field * check_field(const std::string& name , uint8 type) const;
	} ;
}

#define DEFINE_FIELD(h , c , m) h.add(#m , c.m.type())

#endif  /** __SS_TUPLE_H */
