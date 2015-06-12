
#ifndef __SS_FIELD_H
#define __SS_FIELD_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "SSType.h"

namespace ss{

    typedef struct __field_holder{
        uint8   type ;
        uint8   flag ;              //0:表示原始状态NotAssigned、CanAssign，1表示已经赋值 ，1: 1表示不能赋值；2:表示是否自动释放，0表示自动释放。
                                    //3: modify flag. 4: insert flag 5:delete flag
        uint16  magic ;             //魔数，主要为了字节对齐
        uint32  length ;            //数据长度
        union{
            int8        i8  ;
            uint8       u8  ;
            int16       i16 ;
            uint16      u16 ;
            int32       i32 ;
            uint32      u32 ;
            int64       i64 ;
            uint64      u64 ;
            bool        bv  ;
            char *      str ;
            float       f32 ;
            double      f64 ;
            void *      raw ;

			time_t		t32 ;
			int64		t64 ;

            uint64      val ;       //用来清除数值的
        }data ;    
    } field_holder;

	const int FIELD_ASSIGNED_INDEX	=	0 ;
	const int FIELD_CAN_INDEX		=	1 ;
	const int FIELD_AUTOFREE_INDEX	=	2 ;

	inline bool field_assigned(const field_holder& holder) {return get_bit_mask(holder.flag , FIELD_ASSIGNED_INDEX) ;}
	inline bool field_can(const field_holder& holder) {return get_bit_mask(holder.flag , FIELD_CAN_INDEX) == false ;}
	inline bool field_autofree(const field_holder& holder) {return get_bit_mask(holder.flag , FIELD_AUTOFREE_INDEX) == false ;}

	/*
		2012-06-15
		对于field和其他类型的field来说，本身等同于一个struct，作为数据的访问接口，
		所以不能包含虚函数。由于没有成员变量，因此不会改变field的内存布局。
	*/
    class field{
    public:
        field() ;
        field(const field& field) ;
        ~field() ;
    
        inline uint8 type() const {return holder_.type ;}
        void type(uint8 t) ;

        inline bool assigned() const {return get_bit_mask(holder_.flag , 0);}
        inline bool can() const {return get_bit_mask(holder_.flag , 1) == 0 ; }
        inline void can(bool c) {set_bit_mask(holder_.flag , 1 , !c) ;}

        inline uint32 length() const {return holder_.length ;}
        const char * buffer() const ;
		field_holder * holder() {return &holder_;}
        /*
            2012-06-12
            清掉数据，同时置为NULL
        */
        void clear() ;

        /*
            2012-06-12
            类型、长度也一起清掉
        */
        void reset() ;

        bool operator == (const field& f) ;
        bool operator != (const field& f) ;

		static field FIELD_NULL ;
    protected:
        field_holder holder_ ;
    } ;	

    class fieldi8 : public field{
    public:
        fieldi8() ;
        fieldi8(const fieldi8 & i8) ;

        operator int8 () const;

        const fieldi8& operator= (int val) ;
        const fieldi8& operator= (const char * val) ;
    } ;


    class fieldu8 : public field{
    public:
        fieldu8() ;
        fieldu8(const fieldu8 & u8) ;

        operator uint8 () const;

        const fieldu8& operator= (int val) ;
        const fieldu8& operator= (const char * val) ;
    } ;

    class fieldi16 : public field{
    public:
        fieldi16() ;
        fieldi16(const fieldi16 & i8) ;

        operator int16 () const;

        const fieldi16& operator= (int val) ;
        const fieldi16& operator= (const char * val) ;
    } ;


    class fieldu16 : public field{
    public:
        fieldu16() ;
        fieldu16(const fieldu16 & u16) ;

        operator uint16 () const;

        const fieldu16& operator= (int val) ;
        const fieldu16& operator= (const char * val) ;
    } ;


    class fieldi32 : public field{
    public:
        fieldi32() ;
        fieldi32(const fieldi32 & i32) ;

        operator int32 () const;

        const fieldi32& operator= (int val) ;
        const fieldi32& operator= (const char * val) ;
    } ;


    class fieldu32 : public field{
    public:
        fieldu32() ;
        fieldu32(const fieldu32 & u32) ;

        operator uint32 () const;

        const fieldu32& operator= (uint32 val) ;
        const fieldu32& operator= (const char * val) ;
    } ;

    class fieldi64 : public field{
    public:
        fieldi64() ;
        fieldi64(const fieldi64 & i64) ;

        operator int64 () const;

        const fieldi64& operator= (int64 val) ;
        const fieldi64& operator= (const char * val) ;
    } ;


    class fieldu64 : public field{
    public:
        fieldu64() ;
        fieldu64(const fieldu64 & u64) ;

        operator uint64 () const;

        const fieldu64& operator= (uint64 val) ;
        const fieldu64& operator= (const char * val) ;
    } ;

    class fieldbv : public field{
    public:
        fieldbv() ;
        fieldbv(const fieldbv & bv) ;

        operator bool () const;

        const fieldbv& operator= (bool val) ;
        const fieldbv& operator= (const char * val) ;
    } ;

    class fieldf32 : public field{
    public:
        fieldf32() ;
        fieldf32(const fieldf32 & f32) ;

        operator float () const;

        const fieldf32& operator= (float val) ;
        const fieldf32& operator= (const char * val) ;
    } ;

    class fieldf64 : public field{
    public:
        fieldf64() ;
        fieldf64(const fieldf64 & f64) ;

        operator double () const;

        const fieldf64& operator= (double val) ;
        const fieldf64& operator= (const char * val) ;
    } ;


    class fieldstr : public field{
    public:
        fieldstr() ;
        fieldstr(const fieldstr & str) ;

		operator const std::string () const ;

        const fieldstr& operator= (const char * val) ;
        const char * c_str() const;
    } ;

    class fieldraw : public field{
    public:
        fieldraw() ;
        fieldraw(const fieldraw & raw) ;

        operator const void * () const;
        int copy(const void * buffer , uint32 length) ;
    } ;

	class fieldt32 : public field{
	public:
		fieldt32() ;
		fieldt32(const fieldt32& t32) ;

		operator time_t() const ;

		const fieldt32& operator=(time_t val) ;

		/*
			2012-06-20
			只支持时间格式"YYYYMMDD HH:MM:SS"
		*/
		const fieldt32& operator=(const char * val) ;
	} ;

	class fieldt64 : public field{
	public:
		fieldt64() ;
		fieldt64(const fieldt64& t64) ;

		operator int64 () const ;

		const fieldt64& operator=(int64 val) ;

		/*
			2012-06-20
			只支持时间格式"YYYYMMDD HH:MM:SS.mmm"
		*/
		const fieldt64& operator=(const char * val) ;

		time_t get_time() const ;
		int32 get_msec() const ;
	} ;

	class field_def{
	public:
		std::string		name ;
		std::string		table_name;
		uint8			type ;						//数据类型
		unsigned int	decimals ;					//小数点位数
		unsigned long	max_length ;				//row的最大宽度,unit byte
		unsigned long	length;						//字段的长度
		int				index ;						//在纪录中的下标，－1表示非法，从0开始
		int				offset ;					//在纪录中的偏移量

		field_def() ;
		field_def(const field_def& def) ;
		field_def(const std::string& str , const std::string &table, uint8 t , int idx =-1 , int of = -1) ;

        bool operator==(const field_def& def) const;
        bool operator!=(const field_def& def) const;

        static field_def FIELD_NULL ;	
	} ;
}

#endif  /** __SS_FIELD_H */
