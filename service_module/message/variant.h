
#ifndef __SDBUS_VARIANT_H
#define __SDBUS_VARIANT_H 1

#include "sdbus/compile.h"
#include "sdbus/type.h"
#include "sdbus/string.h"

namespace sdbus{


class  Variant ;
class  VariantMap ;
class  VariantList ;

/*
    替代list
*/
class SDBUSAPI VariantList{
public:
    VariantList() ;
    VariantList(const VariantList& list) ;
    virtual ~VariantList() ;
    VariantList& operator=(const VariantList& list) ;

    void Clear() ;
    inline size_t Size() const {return size_ ;}
    inline bool Empty() const {return size_ == 0 ;}

    VariantMap * AddMap() ;
    VariantList * AddList() ;
    Variant * AddVariant() ;
    Variant * AddVariant(int type) ;
    void PushBack(const Variant& var) ;

    const Variant * Get(size_t index) const ;
    Variant * Get(size_t index) ;

    inline const Variant * operator[](size_t index) const {return Get(index) ; }
    inline Variant * operator[](size_t index) {return Get(index) ;}

    inline uint32_t Tag() const {return tag_ ;}
    inline void Tag(uint32_t val) {tag_ = val;}

    void Attach(VariantList& list) ;

    static const VariantList EMPTY ;
protected:
    void copy(const VariantList& list) ;

    Variant * variants_ ;
    size_t size_ ;
    size_t capacity_ ;

    /*
        由于list内部所有的元素共享一个tag。所以，不得不将这个tag置入variant list内部。
    */
    uint32_t tag_ ;

    bool extend() ;
} ;

class FieldDict ;

class SDBUSAPI VariantMap{
public:
    VariantMap() ;
    virtual ~VariantMap() ;

    VariantMap(const VariantMap& map) ;
    VariantMap& operator=(const VariantMap& map) ;


    void Clear() ;
    inline int Size() const {return size_ ;}
    inline int GetFieldCount() const {return size_ ;}
    inline bool Empty() const {return Size() == 0;}
    bool GetFieldInfo(int index , uint32_t& tag , int&  type) const ;
    bool GetFieldInfo(int index , sdbus::string& name , int&  type) const ;
    bool GetFieldValue(int index , const Variant *& var) const ;

    bool SetBool(uint32_t tag, bool value);

    bool SetUInt8(uint32_t tag, uint8_t value);
    bool SetUInt16(uint32_t tag, uint16_t value);
    bool SetUInt32(uint32_t tag, uint32_t value);
    bool SetUInt64(uint32_t tag, uint64_t value);
    bool SetInt8(uint32_t tag, int8_t value);
    bool SetInt16(uint32_t tag, int16_t value);
    bool SetInt32(uint32_t tag, int32_t value);
    bool SetInt64(uint32_t tag, int64_t value);
    bool SetFloat(uint32_t tag, float value);
    bool SetDouble(uint32_t tag, double value);

    bool SetString(uint32_t tag , const char * value) ;
    bool SetString(uint32_t tag, const sdbus::string& value);
    inline bool SetString(uint32_t tag, const std::string& value) {return SetString(tag , value.c_str()) ;}

    bool SetRaw(uint32_t tag, const char *value, size_t size);
    bool SetRaw(uint32_t tag, const sdbus::string& value);
    inline bool SetRaw(uint32_t tag, const std::string& value) {return SetRaw(tag , value.data() , value.length()) ;}

    bool SetMap(uint32_t tag , const VariantMap& map) ;
    bool SetMap(uint32_t tag , VariantMap& map) ;

    bool SetList(uint32_t tag , const VariantList& list) ;
    bool SetList(uint32_t tag , VariantList& list) ;


    inline bool SetBool(const char * name, bool value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetBool(tag , value) ;}

    inline bool SetUInt8(const char * name, uint8_t value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetUInt8(tag , value) ;}
    inline bool SetUInt16(const char * name, uint16_t value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetUInt16(tag , value) ;}
    inline bool SetUInt32(const char * name, uint32_t value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetUInt32(tag , value) ;}
    inline bool SetUInt64(const char * name, uint64_t value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetUInt64(tag , value) ;}

    inline bool SetInt8(const char * name, int8_t value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetInt8(tag , value) ;}
    inline bool SetInt16(const char * name, int16_t value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetInt16(tag , value) ;}
    inline bool SetInt32(const char * name, int32_t value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetInt32(tag , value) ;}
    inline bool SetInt64(const char * name, int64_t value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetInt64(tag , value) ;}

    inline bool SetFloat(const char * name, float value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetFloat(tag , value) ;}
    inline bool SetDouble(const char * name, double value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetDouble(tag , value) ;}

    inline bool SetString(const char * name, const char * value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetString(tag , value) ;}
    inline bool SetString(const char * name, const sdbus::string& value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetString(tag , value) ;}
    inline bool SetString(const char * name, const std::string& value){ return SetString(name , value.c_str()) ;}

    inline bool SetRaw(const char * name, const char * value , size_t size){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetRaw(tag , value , size) ;}
    inline bool SetRaw(const char * name, const sdbus::string& value){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetRaw(tag , value) ;}
    inline bool SetRaw(const char * name, const std::string& value){return SetRaw(name , value.data() , value.length()) ;}

    inline bool SetMap(const char * name, const VariantMap& map){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetMap(tag , map) ;}
    inline bool SetList(const char * name, const VariantList& list){ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return SetList(tag , list) ;}


    /*
        这三个函数专门是为了提高性能，推荐使用。Add之后，会返回内部生成的类，然后直接在这个类上面操作
    */
    VariantMap* AddMap(uint32_t tag) ;
    VariantList* AddList(uint32_t tag) ;
    Variant* AddVariant(uint32_t tag) ;

    inline VariantMap* AddMap(const char * name) {uint32_t tag = Name2Tag(name) ; if(tag == 0) return NULL ; else return AddMap(tag) ;}
    inline VariantList* AddList(const char * name) {uint32_t tag = Name2Tag(name) ; if(tag == 0) return NULL ; else return AddList(tag) ;}
    inline Variant* AddVariant(const char * name) {uint32_t tag = Name2Tag(name) ; if(tag == 0) return NULL ; else return AddVariant(tag) ;}


    bool GetBool(uint32_t tag, bool &value) const;
    bool GetUInt8(uint32_t tag, uint8_t &value) const;
    bool GetUInt16(uint32_t tag, uint16_t &value) const;
    bool GetUInt32(uint32_t tag, uint32_t &value) const;
    bool GetUInt64(uint32_t tag, uint64_t &value) const;
    bool GetInt8(uint32_t tag, int8_t &value) const;
    bool GetInt16(uint32_t tag, int16_t &value) const;
    bool GetInt32(uint32_t tag, int32_t &value) const;
    bool GetInt64(uint32_t tag, int64_t &value) const;
    bool GetFloat(uint32_t tag, float &value) const;
    bool GetDouble(uint32_t tag, double &value) const;

    bool GetString(uint32_t tag, sdbus::string& value) const;
    inline bool GetString(uint32_t tag, std::string& value) const 
    {
        const sdbus::string * val = NULL ;
        if(RefString(tag , val) == false || val == NULL)
            return false ;

        value = val->c_str() ;
        return true ;
    }

    bool GetRaw(uint32_t tag, sdbus::string& value) const;
    inline bool GetRaw(uint32_t tag, std::string& value) const
    {
        const sdbus::string * raw = NULL ;
        if(RefRaw(tag , raw) == false || raw == NULL)
            return false ;

        value.assign(raw->data() , raw->length()) ;
        return true ;
    }

    bool GetMap(uint32_t tag, VariantMap&value) const;
    bool GetList(uint32_t tag , VariantList& value) const;

    inline bool GetBool(const char * name, bool &value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetBool(tag , value) ; }
    inline bool GetUInt8(const char * name, uint8_t &value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetUInt8(tag , value) ; }
    inline bool GetUInt16(const char * name, uint16_t &value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetUInt16(tag , value) ; }
    inline bool GetUInt32(const char * name, uint32_t &value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetUInt32(tag , value) ; }
    inline bool GetUInt64(const char * name, uint64_t &value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetUInt64(tag , value) ; }
    inline bool GetInt8(const char * name, int8_t &value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetInt8(tag , value) ; }
    inline bool GetInt16(const char * name, int16_t &value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetInt16(tag , value) ; }
    inline bool GetInt32(const char * name, int32_t &value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetInt32(tag , value) ; }
    inline bool GetInt64(const char * name, int64_t &value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetInt64(tag , value) ; }
    inline bool GetFloat(const char * name, float &value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetFloat(tag , value) ; }
    inline bool GetDouble(const char * name, double &value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetDouble(tag , value) ; }
    inline bool GetString(const char * name, sdbus::string& value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetString(tag , value) ; }
    inline bool GetString(const char * name, std::string& value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetString(tag , value) ; }
    inline bool GetRaw(const char * name, sdbus::string& value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetRaw(tag , value) ; }
    inline bool GetRaw(const char * name, std::string& value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetRaw(tag , value) ; }
    inline bool GetMap(const char * name, VariantMap&value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetMap(tag , value) ; }
    inline bool GetList(const char * name , VariantList& value) const{ uint32_t tag = Name2Tag(name) ; if(tag == 0) return false ; else return GetList(tag , value) ; }


    bool RefString(uint32_t tag, const char *& value, size_t &size) const;
    bool RefString(uint32_t tag, const sdbus::string *& value) const;

    bool RefRaw(uint32_t tag, const void *& value, size_t &size) const;
    bool RefRaw(uint32_t tag, const sdbus::string *& value) const;

    bool RefMap(uint32_t tag, const VariantMap* &value) const;
    bool RefList(uint32_t tag , const VariantList*& value) const;    
    bool RefVar(uint32_t tag, const Variant* &value) const;


    bool RefVar(uint32_t tag, Variant*& value) ;
    bool RefMap(uint32_t tag, VariantMap* &value) ;
    bool RefList(uint32_t tag , VariantList*& value) ;    

    inline bool RefString(const char * name, const char *& value, size_t &size) const{ uint32_t tag = Name2Tag(name); if(tag == 0) return false ;else return RefString(tag , value , size);}
    inline bool RefString(const char * name, const sdbus::string*& value) const{ uint32_t tag = Name2Tag(name); if(tag == 0) return false ;else return RefString(tag , value);}

    inline bool RefRaw(const char * name, const void *& value, size_t &size) const{ uint32_t tag = Name2Tag(name); if(tag == 0) return false ;else return RefRaw(tag , value , size);}
    inline bool RefRaw(const char * name, const sdbus::string* value) const{ uint32_t tag = Name2Tag(name); if(tag == 0) return false ;else return RefRaw(tag , value);}

    inline bool RefVar(const char * name, const Variant* &value) const{ uint32_t tag = Name2Tag(name); if(tag == 0) return false ;else return RefVar(tag , value);}
    inline bool RefMap(const char * name, const VariantMap* &value) const{ uint32_t tag = Name2Tag(name); if(tag == 0) return false ;else return RefMap(tag , value);}
    inline bool RefList(const char * name , const VariantList*& value) const{ uint32_t tag = Name2Tag(name); if(tag == 0) return false ;else return RefList(tag , value);}

    inline bool RefVar(const char * name, Variant* &value){ uint32_t tag = Name2Tag(name); if(tag == 0) return false ;else return RefVar(tag , value);}
    inline bool RefMap(const char * name, VariantMap* &value){ uint32_t tag = Name2Tag(name); if(tag == 0) return false ;else return RefMap(tag , value);}
    inline bool RefList(const char * name , VariantList*& value) { uint32_t tag = Name2Tag(name); if(tag == 0) return false ;else return RefList(tag , value);} 

    static const VariantMap EMPTY ;

    /*
        AddXXX是为了解码用的，请勿直接使用
    */
    bool AddUInt64(uint32_t tag , uint64_t val) ;
    bool AddFloat(uint32_t tag , float val) ;
    bool AddDouble(uint32_t tag , double val) ;
    bool AddRaw(uint32_t tag , sdbus::string& raw) ;

    void Attach(VariantMap& map) ;

    uint32_t Name2Tag(const char * name) const;
    const char * Tag2Name(uint32_t tag) const ;

    inline const FieldDict * Dict() const {return dict_ ;}
    inline void Dict(const FieldDict * dict) {dict_ = dict ;}
protected :

    Variant * variants_ ;
    int size_ ;
    int capacity_ ;
    mutable int last_index_ ;

    const FieldDict * dict_ ;

    bool extend() ;
    void copy(const VariantMap& map) ;

    bool get_value(uint32_t tag , const Variant*& var) const ;
    bool get_value(uint32_t tag , Variant*& var) ;

    bool get_index(uint32_t tag , int& index) const ;

    void init() ;
} ;


class SDBUSAPI Variant{
public:

    typedef union __st_variant_data 
    {
        int8_t          i8  ;
        uint8_t         u8  ;
        int16_t         i16 ;
        uint16_t        u16 ;
        int32_t         i32 ;
        uint32_t        u32 ;
        int64_t         i64 ;
        uint64_t        u64 ;

        bool            bv  ;
        float           f32 ;
        double          f64 ;

        sdbus::string*    str ;
        sdbus::string*    raw ;

        VariantMap*     map ;
        VariantList*    list;

        uint64_t        val ;
    } VariantData ;

    static const int16_t VAR_MIN      = 0 ;
    static const int16_t  VAR_UNKNOWN = 0  ;
    static const int16_t  VAR_VOID = VAR_UNKNOWN ;

    static const int16_t  VAR_BOOL    = 1 ;

    static const int16_t  VAR_INT8    = 2 ;
    static const int16_t  VAR_UINT8   = 3 ;

    static const int16_t  VAR_INT16   = 4 ;
    static const int16_t  VAR_UINT16  = 5 ;
    static const int16_t  VAR_INT32   = 6 ;
    static const int16_t  VAR_UINT32  = 7 ;
    static const int16_t  VAR_INT64   = 8 ;
    static const int16_t  VAR_UINT64  = 9 ;

    static const int16_t  VAR_FLOAT   = 10 ;
    static const int16_t  VAR_DOUBLE  = 11 ;

    static const int16_t  VAR_STRING  = 12 ;        //str16
    static const int16_t  VAR_RAW     = 13 ;        //vbin32

    static const int16_t  VAR_MAP     = 14 ;        //map
    static const int16_t  VAR_LIST    = 15 ;        //list
    static const int16_t  VAR_MAX     = 15 ;    


    static const char * Type2Desc(int type) ;


    mutable int16_t     Type ;
    mutable int16_t     Flag ;
    mutable uint32_t    Tag ;           //字段的标记
    mutable VariantData Data ;


    typedef VariantMap Map ;
    typedef VariantList List ;

    Variant()  ;
    Variant(const Variant& var) ;
    Variant(int type , uint32_t tag) ;

    Variant(bool val) ;
    Variant(int8_t val) ;
    Variant(uint8_t val) ;
    Variant(int16_t val) ;
    Variant(uint16_t val) ;
    Variant(int32_t val) ;
    Variant(uint32_t val) ;
    Variant(int64_t val) ;
    Variant(uint64_t val) ;

    Variant(float val) ;
    Variant(double val) ;

    Variant(const char * val) ;
    Variant(const sdbus::string& val) ;
    Variant(const void * val , int len) ;

    Variant(const Map& map) ;
    Variant(const List& list) ;
    

    ~Variant()  ;

    /*
        Clear只清空数据，Reset除了清空数据外，还重置状态。
    */
    void Clear() ;
    void Reset() ;

    bool Assigned() const ;
    void Assigned(bool val) ;

    /*
        请注意，下面的Set和Get都有严格的类型匹配，比如bool类型Variant只能存取bool类型的值。
    */

    bool SetBool(bool val) ;
    bool SetInt8(int8_t val) ;
    bool SetUInt8(uint8_t val) ;
    bool SetInt16(int16_t val) ;
    bool SetUInt16(uint16_t val) ;
    bool SetInt32(int32_t val) ;
    bool SetUInt32(uint32_t val) ;
    bool SetInt64(int64_t val) ;
    bool SetUInt64(uint64_t val) ;

    bool SetFloat(float val) ;
    bool SetDouble(double val) ;

    bool SetString(const char * val) ;
    bool SetString(const sdbus::string& val) ;
    inline bool SetString(const std::string& val) {return SetString(val.c_str()) ;}

    bool SetRaw(const void * val , int len) ;
    bool SetRaw(const sdbus::string& val) ;
    inline bool SetRaw(const std::string& val) {return SetRaw(val.data() , val.length()) ;}

    bool SetMap(const Map& map) ;
    bool SetMap(Map& map) ;

    bool SetList(const List& list) ;
    bool SetList(List& list) ;

    /*
        使用Get接口，直接需要内存拷贝。并自动进行类型转化。
    */
    bool GetBool(bool& val) const;
    bool GetInt8(int8_t& val) const;
    bool GetUInt8(uint8_t& val) const;
    bool GetInt16(int16_t& val) const;
    bool GetUInt16(uint16_t& val) const;
    bool GetInt32(int32_t& val) const;
    bool GetUInt32(uint32_t& val) const;
    bool GetInt64(int64_t& val) const;
    bool GetUInt64(uint64_t& val) const;

    bool GetFloat(float& val) const;
    bool GetDouble(double& val) const;

    bool GetString(sdbus::string& val) const;
    inline bool GetString(std::string& val) const
    {
        const sdbus::string * str = NULL ;
        if(RefString(str) == false || str == NULL)
            return false ;

        val = str->c_str() ;
        return true ;
    }

    bool GetRaw(void * val , size_t& len) const;
    bool GetRaw(sdbus::string& val) const;
    inline bool GetRaw(std::string& val) const
    {
        const sdbus::string * raw = NULL ;
        if(RefRaw(raw) == false || raw == NULL)
            return false ;

        val.assign(raw->data() , raw->length()) ;
        return true ;
    }

    bool GetMap(Map& map) const;
    bool GetList(List& list) const;

    /*  
        直接获取内部数据指针，不拷贝数据。
        由于不进行类型转换，可能会失败。
    */
    bool RefString(const char *& str , size_t& len) const;
    bool RefString(const sdbus::string *& str) const ;
    bool RefString(sdbus::string *& str) ;
    bool RefRaw(const void *& val , size_t & len) const;
    bool RefRaw(const sdbus::string* & raw) const;
    bool RefRaw(sdbus::string* & raw) ;

    bool RefMap(const Map *& map) const;
    bool RefList(const List *& list) const;

    bool RefMap(Map *& map) ;
    bool RefList(List *& list) ;

    Variant& operator=(bool val) ;
    Variant& operator=(int8_t val) ;
    Variant& operator=(uint8_t val) ;
    Variant& operator=(int16_t val) ;
    Variant& operator=(uint16_t val) ;
    Variant& operator=(int32_t val) ;
    Variant& operator=(uint32_t val) ;
    Variant& operator=(int64_t val) ;
    Variant& operator=(uint64_t val) ;

    Variant& operator=(float val) ;
    Variant& operator=(double val) ;

    Variant& operator=(const char * val) ;
    Variant& operator=(const sdbus::string& val) ;
    inline Variant& operator=(const std::string& val) 
    {
        SetString(val) ;
        return (*this) ;
    }

    Variant& operator=(const Map& map) ;
    Variant& operator=(const List& list) ;

    Variant& operator=(const Variant& var) ;


    /*
        没有进行类型转换，有严格的类型判断，不建议使用。
    */
    operator bool() const ;

    operator int8_t() const ;
    operator uint8_t() const ;
    operator int16_t() const ;
    operator uint16_t() const ;
    operator int32_t() const ;
    operator uint32_t() const ;
    operator int64_t() const ;
    operator uint64_t() const ;

    operator float() const ;
    operator double() const ;

    operator const sdbus::string & () const ;  
    operator const Map& () const ;        
    operator const List& () const ;       

    void Assign(const Variant& var) ;


    /*
        自动进行类型转换，会损失性能，但方便使用。
    */
    bool ToBool() const ;
    int32_t ToInt32() const ;
    int64_t ToInt64() const ;

    uint32_t ToUInt32() const ;
    uint64_t ToUInt64() const ;

    float ToFloat() const ;
    double ToDouble() const ;

    const sdbus::string ToString() const ;
    const Map& ToMap() const ;
    const List& ToList() const ;

    static const Variant EMPTY ;


    //bool CheckInteger() const ;
    //bool CheckDecimal() const ;


    /*
        由于在protobuf的消息流中，list没有明显的定义。
        当多个tag在同一个消息中出现时，则被认定为list。
        AddXXX函数只能在decode时被用到，其他地方勿用。
    */
    bool AddUInt64(uint64_t val) ;
    bool AddFloat(float val) ;
    bool AddDouble(double val) ;
    bool AddRaw(sdbus::string& raw) ;

private:
    /*
        强制清除
    */
    void inner_init() ;

    void to_list() const ;

    /*
        判断是否是以\0结束
    */
    bool check_string(const sdbus::string * str) const;
} ;


}


#endif  /** __SDBUS_VARIANT_H */
