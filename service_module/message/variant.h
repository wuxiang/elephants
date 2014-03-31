#ifndef __VARIANT_H
#define __VARIANT_H

//#include <stdint.h>
//
//#include <string>
//#include <list>
//#include <vector>
//
//namespace Elephants {
//
//#if defined(WIN32) || defined(_WINDOWS)
//
//typedef unsigned char       uint8_t;
//typedef signed char         int8_t;
//typedef unsigned short      uint16_t;
//typedef short               int16_t;
//typedef unsigned int        uint32_t;
//typedef int                 int32_t;
//typedef unsigned __int64    uint64_t;
//typedef __int64             int64_t;
//
//#else
//#include <stdint.h>
//typedef ::uint8_t uint8_t ;
//typedef ::int8_t  int8_t ;
//typedef ::uint16_t uint16_t ;
//typedef ::int16_t  int16_t ;
//typedef ::uint32_t uint32_t ;
//typedef ::int32_t  int32_t ;
//typedef ::uint64_t uint64_t ;
//typedef ::int64_t  int64_t ; 
//#endif
//
//class  Variant ;
//class  VariantRaw ;
//class  VariantMap ;
//class  VariantList ;
//
///*
//    代替Raw
//*/
//class  VariantRaw{
//public:
//    void * Data ;
//    uint32_t Length ;
//
//    VariantRaw() ;
//    VariantRaw(const VariantRaw& raw) ;
//    ~VariantRaw() ;
//
//    bool Copy(const VariantRaw& raw) ;
//    bool Copy(const void * buffer , size_t buflen) ;
//
//    void Ref(const VariantRaw& raw) ;
//    void Clear() ;
//
//    VariantRaw& operator=(const VariantRaw& raw) ;
//    inline bool Owned() const {return owned_ ;}
//private:
//    bool owned_ ;       //是否由这个类自动释放。
//} ;
//
///*
//    替代list
//*/
//class  VariantList{
//public:
//    VariantList() ;
//    VariantList(const VariantList& list) ;
//    virtual ~VariantList() ;
//
//    inline size_t size() const {return size_ ;}
//
//    /*
//        新函数，建议使用
//    */
//    VariantMap* add_map() ;
//    VariantList* add_list() ;
//    Variant* add_variant() ;
//
//    inline void Clear() {clear() ;}
//
//    inline VariantMap * AddMap() {return add_map() ;}
//    inline VariantList * AddList() {return add_list() ;}
//    inline Variant * AddVariant() {return add_variant() ;}
//
//    VariantList& operator=(const VariantList& list) ;
//
//    class iterator ;
//    class  const_iterator { 
//    public:
//        const_iterator() ;
//        const_iterator(const const_iterator& iter) ;
//        const_iterator(const iterator& iter) ;
//
//        const_iterator& operator=(const const_iterator& iter) ;
//        const_iterator& operator=(const iterator& iter) ;
//
//        bool operator==(const const_iterator& iter) const ;
//        bool operator!=(const const_iterator& iter) const ;
//        bool operator==(const iterator& iter) const ;
//        bool operator!=(const iterator& iter) const ;
//        const Variant * operator->() const;
//        const Variant& operator*() const;
//        const_iterator& operator++() ;
//        const_iterator operator++(int step) ;
//        const_iterator& operator--() ;
//        const_iterator operator--(int step) ;
//
//        operator bool() const ;
//
//        friend class VariantList ;
//    protected:
//        Variant ** variants_ ;
//        int index_ ;
//        int size_ ;
//    } ;
//    
//    class  iterator : public const_iterator{ 
//    public:
//        iterator() ;
//        iterator(const iterator& iter) ;
//        iterator& operator=(const iterator& iter) ;
//
//        bool operator==(const iterator& iter) const ;
//        bool operator!=(const iterator& iter) const ;
//        Variant * operator->() ;
//        /*
//            如果var为空，会崩溃，所以在使用*之前，必须检查
//        */
//        Variant& operator*();
//        iterator& operator++() ;
//        iterator operator++(int step) ;
//        iterator& operator--() ;
//        iterator operator--(int step) ;
//    } ;
//
//    /*
//        新函数，建议使用
//    */
//    inline const Variant * operator[](size_t index) const 
//    {
//        if(variants_ == NULL || index >= capacity_)
//            return false ;
//
//        return variants_[index] ;
//    }
//
//    inline Variant * operator[](size_t index)  
//    {
//        if(variants_ == NULL || index >= capacity_)
//            return false ;
//
//        return variants_[index] ;
//    }
//
//    static const VariantList EMPTY ;
//private:
//    void copy(const VariantList& list) ;
//
//    Variant ** variants_ ;
//    size_t size_ ;
//    size_t capacity_ ;
//
//    //不支持拷贝构造
//    bool extend() ;
//} ;
//
///*
//    替代map，作为message的基类
//*/
//class name_table ;
//class  VariantMap{
//public:
//    VariantMap() ;
//    virtual ~VariantMap() ;
//
//    VariantMap(const VariantMap& map) ;
//
//    VariantMap& operator=(const VariantMap& map) ;
//
//    void SetBool(const char *name, bool value);
//
//    void SetUInt8(const char *name, uint8_t value);
//    void SetUInt16(const char *name, uint16_t value);
//    void SetUInt32(const char *name, uint32_t value);
//    void SetUInt64(const char *name, uint64_t value);
//    void SetInt8(const char *name, int8_t value);
//    void SetInt16(const char *name, int16_t value);
//    void SetInt32(const char *name, int32_t value);
//    void SetInt64(const char *name, int64_t value);
//    void SetFloat(const char *name, float value);
//    void SetDouble(const char *name, double value);
//
//    void SetString(const char *name, const std::string& value);
//    void SetRaw(const char *name, const char *value, size_t size);
//    void SetMap(const char * name , const VariantMap& map) ;
//    void SetList(const char * name , const VariantList& list) ;
//
//    /*
//        这三个函数专门是为了提高性能，推荐使用。Add之后，会返回内部生成的类，然后直接在这个类上面操作
//    */
//    VariantMap* AddMap(const char *name) ;
//    VariantList* AddList(const char *name) ;
//    Variant* AddVariant(const char *name) ;
//
//    //主要是解码的时候用
//    Variant* AddVariant(const char *name , size_t nlen) ;
//
//
//
//    bool GetBool(const char *name, bool &value) const;
//    bool GetUInt8(const char *name, uint8_t &value) const;
//    bool GetUInt16(const char *name, uint16_t &value) const;
//    bool GetUInt32(const char *name, uint32_t &value) const;
//    bool GetUInt64(const char *name, uint64_t &value) const;
//    bool GetInt8(const char *name, int8_t &value) const;
//    bool GetInt16(const char *name, int16_t &value) const;
//    bool GetInt32(const char *name, int32_t &value) const;
//    bool GetInt64(const char *name, int64_t &value) const;
//    bool GetFloat(const char *name, float &value) const;
//    bool GetDouble(const char *name, double &value) const;
//
//    bool GetString(const char *name, std::string& value) const;
//    bool GetRaw(const char *name, VariantRaw& value) const;
//    bool GetMap(const char *name, VariantMap&value) const;
//    bool GetList(const char * name , VariantList& value) const;
//
//    /*
//        内存引用模式，必须在Message析构之前，释放引用。
//    */
//    bool RefString(const char *name, const char *& value, size_t &size) const;
//    bool RefRaw(const char *name, const void *& value, size_t &size) const;
//    bool RefMap(const char *name, const VariantMap* &value) const;
//    bool RefList(const char * name , const VariantList*& value) const;    
//
//    bool RefVar(const char *name, Variant*& value) ;
//    bool RefMap(const char *name, VariantMap* &value) ;
//    bool RefList(const char * name , VariantList*& value) ;    
//
//
//    Variant& operator[](const std::string& name) ;
//    Variant& operator[](const char * name) ;
//
//    class VariantMapItem{
//    public:
//        char * first ;
//        Variant * second ;
//
//        VariantMapItem() ;
//        ~VariantMapItem() ;
//
//        VariantMapItem(const char *name , Variant * data) ;
//        void Value(const char * name , Variant * data) ;
//        void Value(const char * name , size_t nlen , Variant * data) ;
//        void Final() ;
//    } ;
//
//    typedef VariantMapItem Item ;
//
//    class iterator ;
//
//    class  const_iterator { 
//    public:
//        const_iterator() ;
//
//        const_iterator(const const_iterator& iter) ;
//        const_iterator(const iterator& iter) ;
//
//        const_iterator& operator=(const const_iterator& iter) ;
//        const_iterator& operator=(const iterator& iter) ;
//
//        bool operator==(const const_iterator& iter) const ;
//        bool operator!=(const const_iterator& iter) const ;
//
//        bool operator==(const iterator& iter) const ;
//        bool operator!=(const iterator& iter) const ;
//
//        const Item* operator->() const;
//        const_iterator& operator++() ;
//        const_iterator operator++(int step) ;
//        const_iterator& operator--() ;
//        const_iterator operator--(int step) ;
//
//        friend class VariantMap ;
//    protected:
//        Item * items_ ;
//        int index_ ;
//        int size_ ;
//    } ;
//    
//    class  iterator  : public const_iterator{ 
//    public:
//        iterator() ;
//        iterator(const iterator& iter) ;
//        iterator& operator=(const iterator& iter) ;
//
//        bool operator==(const iterator& iter) const ;
//        bool operator!=(const iterator& iter) const ;
//        Item* operator->() ;
//        iterator& operator++() ;
//        iterator operator++(int step) ;
//        iterator& operator--() ;
//        iterator operator--(int step) ;
//    } ;
//
//    const_iterator begin() const ;
//    iterator begin() ;
//
//    const_iterator end() const ;
//    iterator end() ;
//
//    void clear() ;
//    inline bool empty() const {return (size_ == 0 || items_ == NULL) ;}
//
//    iterator find(const std::string& name) ;
//    iterator find(const char * name) ;
//
//    const_iterator find(const std::string& name) const;
//    const_iterator find(const char * name) const;
//
//    /*
//        提取每个字段的信息
//    */
//    inline size_t size() const {return size_ ;}
//    const char * names(size_t index) const ;
//    const Variant * values(size_t index) const ;
//    Variant * values(size_t index) ;
//
//    inline int FieldCount() const {return size_ ;}
//    bool GetFieldInfo(int index , std::string& name , int& type) const ;
//
//    static const VariantMap EMPTY ;
//
//    friend class Decoder ;
//
//    void rebuild_name_table() ;
//
//private:    
//    Item * items_ ;
//    size_t size_ ;
//    size_t capacity_ ;
//
//    mutable name_table * name_table_ ;      //只有在get的时候才生成
//    mutable bool item_changed_ ;
//
//    bool extend() ;
//    void copy(const VariantMap& table) ;
//    bool set_value(const char * name , Variant * var) ;
//    bool get_value(const char * name , const Variant*& var) const ;
//    bool get_value(const char * name , Variant*& var)  ;
//
//    bool set_value(const char * name , size_t nlen , Variant * var) ;
//    bool get_value(const char * name , size_t nlen , const Variant*& var) const ;
//
//    bool get_index(const char * name , int& index) const;
//
//    bool build_name_table() const;
//} ;
//
//
//class  Variant{
//public:
//
//    typedef union __st_variant_data 
//    {
//        int8_t          i8  ;
//        uint8_t         u8  ;
//        int16_t         i16 ;
//        uint16_t        u16 ;
//        int32_t         i32 ;
//        uint32_t        u32 ;
//        int64_t         i64 ;
//        uint64_t        u64 ;
//        bool            bv  ;
//        float           f32 ;
//        double          f64 ;
//        char *          str ;
//        void *          raw ;
//
//        uint64_t        val ;
//    } VariantData ;
//
//    typedef enum 
//    {
//
//        VAR_UNKNOWN = 0 ,
//        VAR_VOID = VAR_UNKNOWN ,
//
//        VAR_BOOL    = 1 ,
//
//        VAR_INT8    = 2 ,
//        VAR_UINT8   = 3 ,
//
//        VAR_INT16   = 4 ,
//        VAR_UINT16  = 5 ,
//        VAR_INT32   = 6 ,
//        VAR_UINT32  = 7 ,
//        VAR_INT64   = 8 ,
//        VAR_UINT64  = 9 ,
//
//        VAR_FLOAT   = 10 ,
//        VAR_DOUBLE  = 11 ,
//
//        VAR_STRING  = 12 ,        //str16
//        VAR_RAW     = 13 ,        //vbin32
//
//        VAR_MAP     = 14 ,        //map
//        VAR_LIST    = 15          //list
//
//    } VarType ;
//
//    VarType    Type ;
//    uint32_t    Length ;
//    VariantData Data ;
//
//    typedef VariantMap Map ;
//    typedef VariantList List ;
//    typedef VariantRaw Raw ;
//
//    Variant()  ;
//    Variant(const Variant& var) ;
//    Variant(VarType type) ;
//
//    Variant(bool val) ;
//    Variant(int8_t val) ;
//    Variant(uint8_t val) ;
//    Variant(int16_t val) ;
//    Variant(uint16_t val) ;
//    Variant(int32_t val) ;
//    Variant(uint32_t val) ;
//    Variant(int64_t val) ;
//    Variant(uint64_t val) ;
//
//    Variant(float val) ;
//    Variant(double val) ;
//
//    Variant(const char * val) ;
//    Variant(const std::string& val) ;
//    Variant(const void * val , int len) ;
//
//    Variant(const Map& map) ;
//    Variant(const List& list) ;
//    
//
//    ~Variant()  ;
//
//    void Clear() ;
//
//    /*
//        请注意，下面的Set和Get都有严格的类型匹配，比如bool类型Variant只能存取bool类型的值。
//    */
//
//    bool SetBool(bool val) ;
//    bool SetInt8(int8_t val) ;
//    bool SetUInt8(uint8_t val) ;
//    bool SetInt16(int16_t val) ;
//    bool SetUInt16(uint16_t val) ;
//    bool SetInt32(int32_t val) ;
//    bool SetUInt32(uint32_t val) ;
//    bool SetInt64(int64_t val) ;
//    bool SetUInt64(uint64_t val) ;
//
//    bool SetFloat(float val) ;
//    bool SetDouble(double val) ;
//
//    bool SetString(const char * val) ;
//    bool SetString(const char * val , size_t len) ;
//    bool SetString(const std::string& val) ;
//    bool SetRaw(const void * val , int len) ;
//
//    bool SetMap(const Map& map) ;
//    bool SetList(const List& list) ;
//
//    bool GetBool(bool& val) const;
//    bool GetInt8(int8_t& val) const;
//    bool GetUInt8(uint8_t& val) const;
//    bool GetInt16(int16_t& val) const;
//    bool GetUInt16(uint16_t& val) const;
//    bool GetInt32(int32_t& val) const;
//    bool GetUInt32(uint32_t& val) const;
//    bool GetInt64(int64_t& val) const;
//    bool GetUInt64(uint64_t& val) const;
//
//    bool GetFloat(float& val) const;
//    bool GetDouble(double& val) const;
//
//    bool GetString(std::string& val) const;
//    bool GetRaw(void * val , size_t& len) const;
//    bool GetRaw(Raw& val) const;
//
//    bool GetMap(Map& map) const;
//    bool GetList(List& list) const;
//
//    /*  
//        直接获取内部数据指针，不拷贝数据
//    */
//    bool RefString(const char *& str , size_t& len) const;
//    bool RefRaw(const void *& val , size_t & len) const;
//
//    bool RefMap(const Map *& map) const;
//    bool RefList(const List *& list) const;
//
//    //如果，要修改内容，请直接调用SetXXX，否则长度会失效
//    //bool RefString(char *& str , size_t& len) ;
//    //bool RefRaw(void *& val , size_t & len) ;
//
//    bool RefMap(Map *& map) ;
//    bool RefList(List *& list) ;
//
//    Variant& operator=(bool val) ;
//    Variant& operator=(int8_t val) ;
//    Variant& operator=(uint8_t val) ;
//    Variant& operator=(int16_t val) ;
//    Variant& operator=(uint16_t val) ;
//    Variant& operator=(int32_t val) ;
//    Variant& operator=(uint32_t val) ;
//    Variant& operator=(int64_t val) ;
//    Variant& operator=(uint64_t val) ;
//
//    Variant& operator=(float val) ;
//    Variant& operator=(double val) ;
//
//    Variant& operator=(const char * val) ;
//    Variant& operator=(const std::string& val) ;
//
//    Variant& operator=(const Raw& raw) ;
//
//    Variant& operator=(const Map& map) ;
//    Variant& operator=(const List& list) ;
//    Variant& operator=(const Variant& var) ;
//
//    operator bool() const ;
//
//    void Assign(const Variant& var) ;
//
//    static const Variant EMPTY ;
//} ;
//
//
//}


#endif  /** __MSGBUS_VARIANT_H */
