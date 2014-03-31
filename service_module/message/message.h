#ifndef __MSGBUS_MESSAGE_H
#define __MSGBUS_MESSAGE_H  1


//#include <string>
//#include <vector>
//
//#include "variant.h"
//
//namespace Elephants {
//
//class  Message {
//public:
//    typedef Variant::VarType Type ;
//    typedef Variant::Map    Map ;
//    typedef Variant::List   List ;
//    typedef Variant::Raw    Raw ;
//
//    static const int FIELD_VOID = Variant::VAR_VOID ;
//
//    static const int FIELD_BOOL = Variant::VAR_BOOL ;
//
//    static const int FIELD_UINT8 = Variant::VAR_UINT8 ;
//    static const int FIELD_UINT16 = Variant::VAR_UINT16 ;
//    static const int FIELD_UINT32 = Variant::VAR_UINT32 ;
//    static const int FIELD_UINT64 = Variant::VAR_UINT64 ;
//
//    static const int FIELD_INT8 = Variant::VAR_INT8 ;
//    static const int FIELD_INT16 = Variant::VAR_INT16 ;
//    static const int FIELD_INT32 = Variant::VAR_INT32 ;
//    static const int FIELD_INT64 = Variant::VAR_INT64 ;
//
//    static const int FIELD_FLOAT = Variant::VAR_FLOAT ;
//    static const int FIELD_DOUBLE = Variant::VAR_DOUBLE ;
//
//    static const int FIELD_STRING = Variant::VAR_STRING ;
//    static const int FIELD_RAW = Variant::VAR_RAW ;
//    static const int FIELD_MESSAGE = Variant::VAR_MAP ;
//    static const int FIELD_LIST = Variant::VAR_LIST ;
//
//    Message();
//    Message(const Message& msg);
//    ~Message();
//
//    inline int FieldCount() const {return map_values_.size() ;}
//    bool GetFieldInfo(int index , const char *& name , Type& type) const ;
//
//    inline const std::string& GetType() const {return type_ ;}
//    inline void SetType(const std::string& type) {type_ = type;}
//
//    inline const std::string& GetSubject() const {return subject_ ;}
//    inline void SetSubject(const std::string& subject) {subject_ = subject;}
//
//    inline const std::string& GetMessageID() const {return message_id_ ;}
//    inline void SetMessageID(const std::string& id) {message_id_ = id;}
//
//    inline const std::string& GetReplyTo() const {return reply_to_ ;}
//    inline void SetReplyTo(const std::string& reply) {reply_to_ = reply;}
//
//    Message& operator=(const Message& msg) ;
//
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
//    void SetString(const char *name, const std::string& value);
//    void SetRaw(const char *name, const char *value, size_t size);
//    void SetMap(const char * name , const Map& map) ;
//    void SetList(const char * name , const List& list) ;
//
//    /*
//        这三个函数专门是为了提高性能，推荐使用。Add之后，会返回内部生成的类，然后直接在这个类上面操作
//    */
//    VariantMap* AddMap(const char *name) ;
//    VariantList* AddList(const char *name) ;
//    Variant* AddVariant(const char *name) ;
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
//    bool GetString(const char *name, std::string& value) const;
//    bool GetRaw(const char *name, Raw& value) const;
//    bool GetMap(const char *name, Map&value) const;
//    bool GetList(const char * name , List& value) const;
//
//    /*
//        内存引用模式，必须在Message析构之前，释放引用。
//        推荐使用
//    */
//    bool RefString(const char *name, const char *& value, size_t &size) const;
//    bool RefRaw(const char *name, const void *& value, size_t &size) const;
//    bool RefMap(const char *name, const Map* &value) const;
//    bool RefList(const char * name , const List*& value) const;    
//
//    bool RefVar(const char *name, Variant* &value) ;
//    bool RefMap(const char *name, Map* &value) ;
//    bool RefList(const char * name , List*& value) ;    
//
//
//    inline Map& MapValue() {return map_values_ ;}
//    inline const Map& MapValue() const {return map_values_ ;}
//
//    void Clear() ;
//
//
//
//    /*
//        兼容旧接口，可能被废弃，请勿使用
//    */
//    inline int fieldCount() const {return FieldCount() ;}
//    inline bool getFieldInfo(int index, std::string &name, Type &type) const
//    {
//        const char * str = NULL ;
//        bool result = GetFieldInfo(index , str , type) ;
//        if(result == true)
//            name = str ;
//        return result ;
//    }
//    
//    inline const std::string& getType() const {return GetType() ;}
//    inline void setType(const std::string& type) {SetType(type) ;}
//    
//    inline const std::string& getSubject() const {return GetSubject() ;}
//    inline void setSubject(const std::string &subject) {SetSubject(subject) ;}
//
//    inline const std::string& getMessageID() const {return GetMessageID() ;}
//    inline void setMessageID(const std::string& id) {SetMessageID(id) ;}
//
//    inline const std::string& getReplyTo() const {return GetReplyTo() ;}
//    inline void setReplyTo(const std::string& reply) {SetReplyTo(reply);}
//
//    inline void setBool(const std::string &name, bool value) {SetBool(name.c_str() , value) ;}
//    inline void setUint8(const std::string &name, uint8_t value) {SetUInt8(name.c_str() , value) ;}
//    inline void setUint16(const std::string &name, uint16_t value) {SetUInt16(name.c_str() , value) ;}
//    inline void setUint32(const std::string &name, uint32_t value) {SetUInt32(name.c_str() , value) ;}
//    inline void setUint64(const std::string &name, uint64_t value) {SetUInt64(name.c_str() , value) ;}
//    inline void setInt8(const std::string &name, int8_t value) {SetInt8(name.c_str() , value) ;}
//    inline void setInt16(const std::string &name, int16_t value) {SetInt16(name.c_str() , value) ;}
//    inline void setInt32(const std::string &name, int32_t value) {SetInt32(name.c_str() , value) ;}
//    inline void setInt64(const std::string &name, int64_t value) {SetInt64(name.c_str() , value) ;}
//    inline void setFloat(const std::string &name, float value) {SetFloat(name.c_str() , value) ;}
//    inline void setDouble(const std::string &name, double value) {SetDouble(name.c_str() , value) ;}
//    inline void setString(const std::string &name, const std::string &value) {SetString(name.c_str() , value) ;}
//    inline void setRaw(const std::string &name, const char *value, size_t size) {SetRaw(name.c_str() , value , size) ;}
//    inline void setMessage(const std::string &name, const Message &value) {SetMap(name.c_str() , value.MapValue()) ;}
//    inline void setMap(const std::string &name, const Map &value) {SetMap(name.c_str() , value) ;}
//    inline void setList(const std::string& name , const List& value)  {SetList(name.c_str() , value) ;}
//
//    inline bool getBool(const std::string &name, bool &value) const {return GetBool(name.c_str() , value) ;}
//    inline bool getUint8(const std::string &name, uint8_t &value) const {return GetUInt8(name.c_str() , value) ;}
//    inline bool getUint16(const std::string &name, uint16_t &value) const {return GetUInt16(name.c_str() , value) ;}
//    inline bool getUint32(const std::string &name, uint32_t &value) const {return GetUInt32(name.c_str() , value) ;}
//    inline bool getUint64(const std::string &name, uint64_t &value) const {return GetUInt64(name.c_str() , value) ;}
//    inline bool getInt8(const std::string &name, int8_t &value) const {return GetInt8(name.c_str() , value) ;}
//    inline bool getInt16(const std::string &name, int16_t &value) const {return GetInt16(name.c_str() , value) ;}
//    inline bool getInt32(const std::string &name, int32_t &value) const {return GetInt32(name.c_str() , value) ;}
//    inline bool getInt64(const std::string &name, int64_t &value) const {return GetInt64(name.c_str() , value) ;}
//    inline bool getFloat(const std::string &name, float &value) const {return GetFloat(name.c_str() , value) ;}
//    inline bool getDouble(const std::string &name, double &value) const {return GetDouble(name.c_str() , value) ;}
//    inline bool getString(const std::string &name, std::string &value) const {return GetString(name.c_str() , value) ;}
//    inline bool getRaw(const std::string &name, const char *&value, size_t &size) const {return RefRaw(name.c_str() , (const void *&)value , size) ;}
//    inline bool getMessage(const std::string &name, Message &value) const 
//    {
//        value.Clear() ;
//        const Map * map = NULL ;
//        if(RefMap(name.c_str() , map) == false || map == NULL)
//            return false ;
//
//        value.MapValue() = *map ;
//        return true ;
//    }
//    inline bool getList(const std::string& name , List& value) const {return GetList(name.c_str() , value) ;}
//    inline bool refRaw(const std::string &name, const void *&value, size_t &size) const {return RefRaw(name.c_str() , value , size) ;}
//
//private:
//    std::string type_ ;
//    std::string subject_ ;
//    std::string message_id_ ;
//    std::string reply_to_ ;
//    
//    Map     map_values_ ;
//
//    void assign(const Message& msg) ;
//    /*
//        暂时不支持List类型的消息格式
//    */
//    //List    list_values_ ;
//};
//
//}
//
#endif    /** __MSGBUS_MESSAGE_H */
