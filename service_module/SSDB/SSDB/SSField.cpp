
#include "SSField.h"

namespace ss {

    field::field()
    {
        memset(&holder_ , 0 , sizeof(holder_)) ;
        holder_.magic = 0X5555 ;
    }

    field::field(const field& field)
    {
        memcpy(&holder_ , &field.holder_ , sizeof(holder_)) ;

        if(get_bit_mask(holder_.flag , 2) == false && get_bit_mask(holder_.flag , 0)== true)
        {
            if(holder_.type == SSTYPE_STRING)
            {
                if(holder_.length > 0 && holder_.data.str != NULL)
                {
                    char * str = (char *)malloc(holder_.length) ;
                    memcpy(str , holder_.data.str , holder_.length) ;
                    holder_.data.str = str ;
                }
            }
            else if(holder_.type == SSTYPE_RAW)
            {
                if(holder_.length > 0 && holder_.data.raw != NULL)
                {
                    char * raw = (char *)malloc(holder_.length) ;
                    memcpy(raw , holder_.data.raw , holder_.length) ;
                    holder_.data.raw = raw ;
                }            
            }
        }
    }

    field::~field()
    {
        clear() ;
    }

    void field::type(uint8 t)
    {
        if(t == holder_.type)
            return ;

        if(t < SSTYPE_MIN || t > SSTYPE_MAX)
            return ;

        reset() ;

        holder_.type = t ;
        holder_.length = get_type_size(t) ;
    }

    const char * field::buffer() const
    {
        if(holder_.type == SSTYPE_STRING)
        {
            return holder_.data.str ;
        }
        else if(holder_.type == SSTYPE_RAW)
        {
            return (const char *)holder_.data.raw ;
        }
        else
        {
            return (char *)&holder_.data ;
        }    
    }

    void field::clear()
    {
        if(get_bit_mask(holder_.flag , 2) == false)
        {
            switch(holder_.type)
            {
            case SSTYPE_STRING :
                ::free(holder_.data.str) ;
                break ;
            case SSTYPE_RAW :
                ::free(holder_.data.raw) ;
                break ;
            default:
                break ;
            }
        }//end if ...   

        //Çå³ýÄÚÈÝ
        switch(holder_.type)
        {
        case SSTYPE_INT8 :
        case SSTYPE_UINT8 :
        case SSTYPE_INT16 :
        case SSTYPE_UINT16 :
        case SSTYPE_INT32 :
        case SSTYPE_UINT32 :
        case SSTYPE_INT64 :
        case SSTYPE_UINT64 :
        case SSTYPE_BOOL :
        case SSTYPE_T32 :
        case SSTYPE_T64 :
        case SSTYPE_BIT64 :
            holder_.data.i64 = 0 ;
            break ;
        case SSTYPE_FLOAT :
            holder_.data.f32 = 0 ;
            break ;
        case SSTYPE_DOUBLE :
            holder_.data.f64 = 0 ;
            break ;
        case SSTYPE_STRING :
            holder_.data.str = NULL ;
            holder_.length = 0 ;
            break ;
        case SSTYPE_RAW :
            holder_.data.raw = NULL ;
            holder_.length = 0 ;
            break ;
        default :
            break ;
        }

        set_bit_mask(holder_.flag , 0 , false) ;
    }

    void field::reset()
    {
        clear() ;
        holder_.flag = 0 ;
        holder_.length = 0 ;
        holder_.type = SSTYPE_UNKNOWN ;    
    }

    bool field::operator!=(const field& f)
    {
        if(holder_.type != f.holder_.type)
            return true ;
        if(holder_.length != f.holder_.length)
            return true ;

        bool ta = get_bit_mask(holder_.flag , 0) ;
        bool fa = get_bit_mask(f.holder_.flag , 0) ;
        if(ta != fa)
            return true ;

        switch(holder_.type)
        {
        case SSTYPE_INT8 :
            if(holder_.data.i8 != f.holder_.data.i8)
                return true ;
            break ;
        case SSTYPE_UINT8 :
            if(holder_.data.u8 != f.holder_.data.u8)
                return true ;
            break ;
        case SSTYPE_INT16 :
            if(holder_.data.i16 != f.holder_.data.i16)
                return true ;
            break ;
        case SSTYPE_UINT16 :
            if(holder_.data.u16 != f.holder_.data.u16)
                return true ;
            break ;

        case SSTYPE_INT32 :
            if(holder_.data.i32 != f.holder_.data.i32)
                return true ;
            break ;
        case SSTYPE_UINT32 :
            if(holder_.data.u32 != f.holder_.data.u32)
                return true ;
            break ;
        case SSTYPE_INT64 :
            if(holder_.data.i64 != f.holder_.data.i64)
                return true ;
            break ;
        case SSTYPE_UINT64 :
        case SSTYPE_BIT64 :
            if(holder_.data.u64 != f.holder_.data.u64)
                return true ;
            break ;
        case SSTYPE_BOOL :
            if(holder_.data.bv != f.holder_.data.bv)
                return true ;
            break ;
        case SSTYPE_STRING :
            if(holder_.data.str == NULL && holder_.data.str != f.holder_.data.str )
                return true ;

            if(f.holder_.data.str == NULL && holder_.data.str != f.holder_.data.str )
                return true ;

            return strcmp(holder_.data.str , f.holder_.data.str) != 0 ;
        case SSTYPE_RAW :
            if(holder_.data.raw == NULL && holder_.data.raw != f.holder_.data.raw )
                return true ;

            if(f.holder_.data.raw == NULL && holder_.data.raw != f.holder_.data.raw )
                return true ;

            if(holder_.length != f.holder_.length)
                return true ;

            return memcmp(holder_.data.raw , f.holder_.data.raw , holder_.length) != 0 ;
        case SSTYPE_FLOAT :
            return float_equal(holder_.data.f32 , f.holder_.data.f32) == false ;
        case SSTYPE_DOUBLE :
            return double_equal(holder_.data.f64 , f.holder_.data.f64) == false;
        case SSTYPE_T32 :
            if(holder_.data.t32 != f.holder_.data.t32)
                return true ;
            break ;
        case SSTYPE_T64 :
            if(holder_.data.t64 != f.holder_.data.t64)
                return true ;
            break ;
        default:
            return false ;
        }

        return false ;
    }

    field field::FIELD_NULL ;

    fieldi8::fieldi8()
    {
        holder_.type = SSTYPE_INT8 ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldi8::fieldi8(const fieldi8 & i8)
    {
        memcpy(&holder_ , &i8.holder_ , sizeof(holder_)) ;
    }

    fieldi8::operator int8 () const
    {
        if(assigned() == false)
            return 0 ;
        else
            return holder_.data.i8 ;
    }

    const fieldi8& fieldi8::operator= (int val)
    {
        if(can() == true)
        {
            set_bit_mask(holder_.flag , 0 , true) ;
            holder_.data.i8 = (int8)val ;
        }

        return (*this) ;
    }

    const fieldi8& fieldi8::operator= (const char * val)
    {
        if(can() == true)
        {
            if(val == NULL)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
                holder_.data.val = 0 ;
            }
            else
            {
                set_bit_mask(holder_.flag , 0 , true) ;
                holder_.data.i8 = (int8)atoi(val) ;
            }
        }

        return (*this) ;
    }

    fieldu8::fieldu8()
    {
        holder_.type = SSTYPE_UINT8 ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldu8::fieldu8(const fieldu8 & u8)
    {
        memcpy(&holder_ , &u8.holder_ , sizeof(holder_)) ;
    }

    fieldu8::operator uint8 () const
    {
        if(assigned() == false)
            return 0 ;
        else
            return holder_.data.u8 ;
    }

    const fieldu8& fieldu8::operator= (int val)
    {
        if(can() == true)
        {
            set_bit_mask(holder_.flag , 0 , true) ;
            holder_.data.u8 = (uint8)val ;
        }

        return (*this) ;
    }

    const fieldu8& fieldu8::operator= (const char * val)
    {
        if(can() == true)
        {
            if(val == NULL)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
                holder_.data.val = 0 ;
            }
            else
            {
                set_bit_mask(holder_.flag , 0 , true) ;
                holder_.data.u8 = (uint8)atoi(val) ;
            }
        }

        return (*this) ;
    }



    fieldi16::fieldi16()
    {
        holder_.type = SSTYPE_INT16 ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldi16::fieldi16(const fieldi16 & i16)
    {
        memcpy(&holder_ , &i16.holder_ , sizeof(holder_)) ;
    }

    fieldi16::operator int16 () const
    {
        if(assigned() == false)
            return 0 ;
        else
            return holder_.data.i16 ;
    }

    const fieldi16& fieldi16::operator= (int val)
    {
        if(can() == true)
        {
            set_bit_mask(holder_.flag , 0 , true) ;
            holder_.data.i16 = (int16)val ;
        }

        return (*this) ;
    }

    const fieldi16& fieldi16::operator= (const char * val)
    {
        if(can() == true)
        {
            if(val == NULL)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
                holder_.data.val = 0 ;
            }
            else
            {
                set_bit_mask(holder_.flag , 0 , true) ;
                holder_.data.i16 = (int16)atoi(val) ;
            }
        }

        return (*this) ;
    }

    fieldu16::fieldu16()
    {
        holder_.type = SSTYPE_UINT16 ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldu16::fieldu16(const fieldu16 & u16)
    {
        memcpy(&holder_ , &u16.holder_ , sizeof(holder_)) ;
    }

    fieldu16::operator uint16 () const
    {
        if(assigned() == false)
            return 0 ;
        else
            return holder_.data.u16 ;
    }

    const fieldu16& fieldu16::operator= (int val)
    {
        if(can() == true)
        {
            set_bit_mask(holder_.flag , 0 , true) ;
            holder_.data.u16 = (uint16)val ;
        }

        return (*this) ;
    }

    const fieldu16& fieldu16::operator= (const char * val)
    {
        if(can() == true)
        {
            if(val == NULL)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
                holder_.data.val = 0 ;
            }
            else
            {
                set_bit_mask(holder_.flag , 0 , true) ;
                holder_.data.u16 = (uint16)atoi(val) ;
            }
        }

        return (*this) ;
    }




    fieldi32::fieldi32()
    {
        holder_.type = SSTYPE_INT32 ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldi32::fieldi32(const fieldi32 & i32)
    {
        memcpy(&holder_ , &i32.holder_ , sizeof(holder_)) ;
    }

    fieldi32::operator int32 () const
    {
        if(assigned() == false)
            return 0 ;
        else
            return holder_.data.i32 ;
    }

    const fieldi32& fieldi32::operator= (int val)
    {
        if(can() == true)
        {
            set_bit_mask(holder_.flag , 0 , true) ;
            holder_.data.i32 = (int32)val ;
        }

        return (*this) ;
    }

    const fieldi32& fieldi32::operator= (const char * val)
    {
        if(can() == true)
        {
            if(val == NULL)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
                holder_.data.val = 0 ;
            }
            else
            {
                set_bit_mask(holder_.flag , 0 , true) ;
                holder_.data.i32 = (int32)atoi(val) ;
            }
        }

        return (*this) ;
    }

    fieldu32::fieldu32()
    {
        holder_.type = SSTYPE_UINT32 ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldu32::fieldu32(const fieldu32 & u32)
    {
        memcpy(&holder_ , &u32.holder_ , sizeof(holder_)) ;
    }

    fieldu32::operator uint32 () const
    {
        if(assigned() == false)
            return 0 ;
        else
            return holder_.data.u32 ;
    }

    const fieldu32& fieldu32::operator= (uint32 val)
    {
        if(can() == true)
        {
            set_bit_mask(holder_.flag , 0 , true) ;
            holder_.data.u32 = (uint32)val ;
        }

        return (*this) ;
    }

    const fieldu32& fieldu32::operator= (const char * val)
    {
        if(can() == true)
        {
            if(val == NULL)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
                holder_.data.val = 0 ;
            }
            else
            {
                set_bit_mask(holder_.flag , 0 , true) ;
                holder_.data.u32 = (uint32)atoi(val) ;
            }
        }

        return (*this) ;
    }



    fieldi64::fieldi64()
    {
        holder_.type = SSTYPE_INT64 ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldi64::fieldi64(const fieldi64 & i64)
    {
        memcpy(&holder_ , &i64.holder_ , sizeof(holder_)) ;
    }

    fieldi64::operator int64 () const
    {
        if(assigned() == false)
            return 0 ;
        else
            return holder_.data.i64 ;
    }

    const fieldi64& fieldi64::operator= (int64 val)
    {
        if(can() == true)
        {
            set_bit_mask(holder_.flag , 0 , true) ;
            holder_.data.i64 = (int64)val ;
        }

        return (*this) ;
    }

    const fieldi64& fieldi64::operator= (const char * val)
    {
        if(can() == true)
        {
            if(val == NULL)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
                holder_.data.val = 0 ;
            }
            else
            {
                set_bit_mask(holder_.flag , 0 , true) ;
                holder_.data.i64 = (int64)atoi(val) ;
            }
        }

        return (*this) ;
    }

    fieldu64::fieldu64()
    {
        holder_.type = SSTYPE_UINT64 ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldu64::fieldu64(const fieldu64 & u64)
    {
        memcpy(&holder_ , &u64.holder_ , sizeof(holder_)) ;
    }

    fieldu64::operator uint64 () const
    {
        if(assigned() == false)
            return 0 ;
        else
            return holder_.data.u64 ;
    }

    const fieldu64& fieldu64::operator= (uint64 val)
    {
        if(can() == true)
        {
            set_bit_mask(holder_.flag , 0 , true) ;
            holder_.data.u64 = (uint64)val ;
        }

        return (*this) ;
    }

    const fieldu64& fieldu64::operator= (const char * val)
    {
        if(can() == true)
        {
            if(val == NULL)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
                holder_.data.val = 0 ;
            }
            else
            {
                set_bit_mask(holder_.flag , 0 , true) ;
                holder_.data.u64 = (uint64)atoi64(val) ;
            }
        }

        return (*this) ;
    }

    fieldbv::fieldbv()
    {
        holder_.type = SSTYPE_BOOL ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldbv::fieldbv(const fieldbv & bv)
    {
        memcpy(&holder_ , &bv.holder_ , sizeof(holder_)) ;
    }

    fieldbv::operator bool () const
    {
        if(assigned() == false)
            return false ;
        else
            return holder_.data.bv ;
    }

    const fieldbv& fieldbv::operator= (bool val)
    {
        if(can() == true)
        {
            set_bit_mask(holder_.flag , 0 , true) ;
            holder_.data.bv = val ;
        }

        return (*this) ;
    }

    const fieldbv& fieldbv::operator= (const char * val)
    {
        if(can() == true)
        {
            if(val == NULL)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
                holder_.data.val = 0 ;
            }
            else
            {
                set_bit_mask(holder_.flag , 0 , true) ;
                if(strcmp(val , "TRUE") == 0 || strcmp(val , "true") == 0)
                    holder_.data.bv = true ;
                else
                    holder_.data.bv = (atoi(val) != 0 ) ;
            }
        }

        return (*this) ;
    }



    fieldf32::fieldf32()
    {
        holder_.type = SSTYPE_FLOAT ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldf32::fieldf32(const fieldf32 & f32)
    {
        memcpy(&holder_ , &f32.holder_ , sizeof(holder_)) ;
    }

    fieldf32::operator float () const
    {
        if(assigned() == false)
            return 0 ;
        else
            return holder_.data.f32 ;
    }

    const fieldf32& fieldf32::operator= (float val)
    {
        if(can() == true)
        {
            set_bit_mask(holder_.flag , 0 , true) ;
            holder_.data.f32 = val ;
        }

        return (*this) ;
    }

    const fieldf32& fieldf32::operator= (const char * val)
    {
        if(can() == true)
        {
            if(val == NULL)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
                holder_.data.val = 0 ;
            }
            else
            {
                set_bit_mask(holder_.flag , 0 , true) ;
                holder_.data.f32 = (float)atof(val) ;
            }
        }

        return (*this) ;
    }

    fieldf64::fieldf64()
    {
        holder_.type = SSTYPE_DOUBLE ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldf64::fieldf64(const fieldf64 & f64)
    {
        memcpy(&holder_ , &f64.holder_ , sizeof(holder_)) ;
    }

    fieldf64::operator double () const
    {
        if(assigned() == false)
            return 0 ;
        else
            return holder_.data.f64 ;
    }

    const fieldf64& fieldf64::operator= (double val)
    {
        if(can() == true)
        {
            set_bit_mask(holder_.flag , 0 , true) ;
            holder_.data.f64 = val ;
        }

        return (*this) ;
    }

    const fieldf64& fieldf64::operator= (const char * val)
    {
        if(can() == true)
        {
            if(val == NULL)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
                holder_.data.val = 0 ;
            }
            else
            {
                set_bit_mask(holder_.flag , 0 , true) ;
                holder_.data.f64 = (double)atof(val) ;
            }
        }

        return (*this) ;
    }

    fieldstr::fieldstr()
    {
        holder_.type = SSTYPE_STRING ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldstr::fieldstr(const fieldstr & str)
    {
        memcpy(&holder_ , &str.holder_ , sizeof(holder_)) ;

        uint32 length = holder_.length ;
        if(length > 0)
        {
            char * pchar = (char *)malloc(length) ;
            if(pchar == NULL)
            {
                holder_.length = 0 ;
                holder_.data.str = NULL ;
                set_bit_mask(holder_.flag , 0 , false) ;
            }
            else
            {
                memcpy(pchar , holder_.data.str , length) ;
                holder_.data.str = pchar ;
            }
        }
    }

    fieldstr::operator const std::string () const
    {
        if(assigned() == false || holder_.data.str == NULL || holder_.length <= 0)
            return "" ;
        else
            return holder_.data.str ;
    }

    const fieldstr& fieldstr::operator= (const char * val)
    {
        if(can() == true)
        {
            clear() ;

            if(val != NULL)
            {
                uint32 length = ::strlen(val) + 1 ;
                char * pchar = (char *)malloc(length) ;
                if(pchar != NULL)
                {
                    memcpy(pchar , val , length) ;
                    holder_.data.str =  pchar;
                    holder_.length = length ;
                    set_bit_mask(holder_.flag , 0 , true) ;
                }
            }
        }

        return (*this) ;
    }

    const char * fieldstr::c_str() const
    {
        if(assigned() == false)
            return NULL ;

        return holder_.data.str ;
    }

    fieldraw::fieldraw()
    {
        holder_.type = SSTYPE_RAW ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldraw::fieldraw(const fieldraw & raw)
    {
        memcpy(&holder_ , &raw.holder_ , sizeof(holder_)) ;

        uint32 length = holder_.length ;
        if(length > 0)
        {
            char * pchar = (char *)malloc(length) ;
            if(pchar == NULL)
            {
                holder_.length = 0 ;
                holder_.data.raw = NULL ;
                set_bit_mask(holder_.flag , 0 , false) ;
            }
            else
            {
                memcpy(pchar , holder_.data.raw , length) ;
                holder_.data.raw = pchar ;
            }
        }
    }

    fieldraw::operator const void * () const
    {
        if(assigned() == false)
            return NULL ;
        else
            return holder_.data.raw ;
    }

    int fieldraw::copy(const void * buffer , uint32 length)
    {
        if(can() == true)
        {
            clear() ;

            if(buffer != NULL && length > 0)
            {                
                void * raw = malloc(length) ;
                if(raw != NULL)
                {
                    memcpy(raw , buffer , length) ;
                    holder_.data.raw =  raw;
                    holder_.length = length ;
                    set_bit_mask(holder_.flag , 0 , true) ;
                }
            }
        }

        return (int)holder_.length ;
    }


    fieldt32::fieldt32()
    {
        holder_.type = SSTYPE_T32 ;
        holder_.length = get_type_size(holder_.type) ;
    }

    fieldt32::fieldt32(const fieldt32& t32)
    {
        holder_.type = t32.holder_.type ;
        holder_.length = t32.holder_.length ;
        holder_.magic = t32.holder_.magic ;
        holder_.flag = t32.holder_.flag ;

        holder_.data.t32 = t32.holder_.data.t32 ;
    }

    fieldt32::operator time_t() const
    {
        return holder_.data.t32 ;
    }

    const fieldt32& fieldt32::operator=(time_t val)
    {
        if(can() == true)
        {
            holder_.data.t32 = val ;
            set_bit_mask(holder_.flag , 0 , true) ;
        }
        return (*this) ;
    }

    const fieldt32& fieldt32::operator=(const char * val)
    {
        if(can() == true)
        {
            time_t t ;
            int msec = 0 ;
            bool result = str2time(val , t , msec) ;
            if(val == NULL || result == false)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
            }
            else
            {	
                holder_.data.t32 = t ;
                set_bit_mask(holder_.flag , 0 , true) ;
            }
        }

        return (*this) ;
    }

    fieldt64::fieldt64()
    {
        holder_.type = SSTYPE_T64 ;
        holder_.length = get_type_size(holder_.type) ;	
    }

    fieldt64::fieldt64(const fieldt64& t64)
    {
        holder_.type = t64.holder_.type ;
        holder_.length = t64.holder_.length ;
        holder_.magic = t64.holder_.magic ;
        holder_.flag = t64.holder_.flag ;

        holder_.data.t64 = t64.holder_.data.t64 ;	
    }

    time_t fieldt64::get_time() const 
    {
        return (time_t)(holder_.data.t64 >> 32) ;
    }

    int32 fieldt64::get_msec() const
    {
        return (int32)((holder_.data.t64 << 32) >> 32);
    }
    
    const fieldt64& fieldt64::operator=(int64 val)
    {
        if(can() ==  true)
        {
            holder_.data.t64 = val ;
            set_bit_mask(holder_.flag , 0 , true) ;
        }
        return (*this) ;
    }

    const fieldt64& fieldt64::operator=(const char * val)
    {
        if(can() == true)
        {
            time_t t ;
            int msec = 0 ;
            bool result = str2time(val , t , msec) ;
            if(val == NULL || result == false)
            {
                set_bit_mask(holder_.flag , 0 , false) ;
            }
            else
            {
                int64 i64 = (int32)t ;
                i64 = i64 << 32 ;
                i64 += msec ;

                holder_.data.t64 = i64 ;

                set_bit_mask(holder_.flag , 0 , true) ;
            }
        }
        return (*this) ;
    }

    field_def::field_def()
    {
        decimals = 0;
        max_length = 0;
        length = 0;
        type = SSTYPE_UNKNOWN ;
        index = -1 ;
        offset = - 1 ;
    }

    field_def::field_def(const field_def& def)
    {    
        name = def.name ;
        type = def.type ;
        table_name = def.table_name;
        index = def.index ;
        offset = def.offset ;
        decimals = def.decimals;
        max_length = def.max_length;
        length = def.length;
    }

    field_def::field_def(const std::string& str , const std::string &table, uint8 t , int idx , int of)
    {
        name = str ;
        type = t ;
        table_name = table;
        index = idx ;
        offset = of ; 
        decimals = 0;
        max_length = 0;
        length = 0;
    }

    bool field_def::operator==(const field_def& def) const
    {
        if(type == def.type && name == def.name)
            return true ;

        return false ;
    }

    bool field_def::operator!=(const field_def& def) const
    {
        return !((*this) == def) ;
    }

    field_def field_def::FIELD_NULL ;

}

