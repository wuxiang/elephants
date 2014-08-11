
#include "sdbus/variant.h"
#include "sdbus/dict.h"
//#include "impl/field_map.h"
#include "sdbus/dict.h"

#include "SSRTL/SSLog.h"

namespace sdbus{

/*
    由于protobuf解码时，无法根据WIRETYPE_LENGTH_DELIMITED来判断list , map , string , raw。
    所以，采用lazy decode方法。在提取数据时，进行解码。
*/

VariantList::VariantList()
{
    variants_ = NULL ;
    size_ = 0 ;
    capacity_ = 0 ;
    tag_ = 0 ;
}

VariantList::VariantList(const VariantList& list)
{
    variants_ = NULL ;
    size_ = 0 ;
    capacity_ = 0 ;
    tag_ = 0 ;

    copy(list) ;
}

VariantList::~VariantList()
{
    Clear() ;
}

bool VariantList::extend()
{
    if(size_ < capacity_)
        return true ;

    int cap = capacity_ ;
    if(cap == 0)
        cap = 32 ;
    else
        cap += 16 ;

    size_t buflen = sizeof(Variant) * cap ;
    Variant * vars = (Variant *)::malloc(buflen) ;
    if(vars == NULL)
        return false ;

    ::memset(vars , 0 , buflen) ;
    ::memcpy(vars , variants_ , sizeof(Variant) * size_) ;

    Variant *  tmp = variants_ ;
    variants_ = vars ;
    capacity_ = cap ;

    ::free(tmp) ;

    return true ;
}

const Variant * VariantList::Get(size_t index) const
{
    if(variants_ == NULL || index >= size_)
        return NULL ;
    else
        return variants_ + index ;
}

Variant * VariantList::Get(size_t index)
{
    if(variants_ == NULL || index >= size_)
        return NULL ;
    else
        return variants_ + index ;
}

void VariantList::PushBack(const Variant& var)
{
    if(extend() == false)
        return ;

    variants_[size_++] = var ;
}

VariantMap * VariantList::AddMap()
{
    if(size_ >= capacity_)
    {
        if(extend() == false)
            return NULL ;
    }

    VariantMap * map = new VariantMap() ;
    Variant& var = variants_[size_++] ;

    var.Type = Variant::VAR_MAP ;
    var.Data.map = map ;
    var.Flag = 1 ;

    return map ;
}

VariantList* VariantList::AddList()
{
    if(size_ >= capacity_)
    {
        if(extend() == false)
            return NULL ;
    }

    VariantList * list = new VariantList() ;
    Variant& var = variants_[size_++] ;

    var.Type = Variant::VAR_LIST ;
    var.Data.list = list ;
    var.Flag = 1 ;

    return list ;
}

Variant* VariantList::AddVariant()
{
    if(size_ >= capacity_)
    {
        if(extend() == false)
            return NULL ;
    }

    Variant& var = variants_[size_++] ;

    var.Type = Variant::VAR_VOID ;

    return &var ;
}

Variant * VariantList::AddVariant(int type) 
{
    Variant * var = AddVariant() ;
    var->Type = type ;
    return var ;
}

VariantList& VariantList::operator=(const VariantList& list)
{
    copy(list) ;
    return (*this) ;
}

void VariantList::Clear()
{
    int s = size_ ;
    int c = capacity_ ;
    Variant * vars = variants_ ;

    size_ = 0 ;
    capacity_ = 0 ;
    variants_ = NULL ;
    tag_ = 0 ;

    if(vars != NULL)
    {
        for(int idx = 0 ; idx < s ; ++idx)
        {
            Variant& var = vars[idx] ;
            var.Clear() ;
        }
        ::free(vars) ;
    }
}

const VariantList VariantList::EMPTY ;

void VariantList::copy(const VariantList& list)
{
    Clear() ;

    size_t cap = list.capacity_ ;
    if(cap == 0)
        return ;

    size_t buflen = sizeof(Variant) * cap ;
    Variant * vars = (Variant *)::malloc(buflen) ;
    if(vars == NULL)
        return ;

    ::memset(vars , 0 , buflen) ;

    size_t s = list.Size() ;

    for(size_t idx = 0 ; idx < s ; ++idx)
    {
        vars[idx].Assign(list.variants_[idx]) ;
    }

    variants_ = vars ;
    capacity_ = cap ;
    size_ = s ;
    tag_  = list.Tag() ;
}

void VariantList::Attach(VariantList& list) 
{
    if(&list == this)
        return ;

    Clear() ;

    variants_ = list.variants_ ;
    size_ = list.size_ ;
    capacity_ = list.capacity_ ;
    tag_ = list.tag_ ;

    list.variants_ = NULL ;
    list.size_ = 0 ;
    list.capacity_ = 0 ;
    list.tag_ = 0 ;
}

VariantMap::VariantMap()
{
    init() ;
    //fmaps_ = new FieldMap() ;
}

VariantMap::~VariantMap()
{
    Clear() ;

    /*
    if(fmaps_ != NULL)
    {
        delete fmaps_ ;
        fmaps_ = NULL ;
    }
    */
}

VariantMap::VariantMap(const VariantMap& map)
{
    init() ;
    copy(map) ;
}

VariantMap& VariantMap::operator=(const VariantMap& map)
{
    copy(map) ;
    return (*this) ;
}

void VariantMap::Clear()
{
    Variant * vars = variants_ ;
    int size = size_ ;
    int cap = capacity_ ;

    variants_ = NULL ;
    size_ = 0 ;
    capacity_ = 0 ;

    //LOGDEBUG("we will clear variant map[%p]" , this) ;
    for(int idx = 0 ; idx < size ; ++idx)
    {
        Variant& var = vars[idx] ;
        var.Clear() ;    
    }

    //LOGDEBUG("we will clear variant map's vars[%p]" , vars) ;
    if(vars != NULL)
    {
        ::free(vars) ;
    }
    //LOGDEBUG("we have clear variant map[%p]" , this) ;
    /*
    if(fmaps_ != NULL)
    {
        fmaps_->Clear() ;
    } 
    */
}

bool VariantMap::GetFieldInfo(int index , uint32_t& tag , int&  type) const
{
    if(index < 0 || index >= size_)
        return false ;

    Variant& var = variants_[index] ;
    tag = var.Tag ;
    type = var.Type ;
    return true ;
}

bool VariantMap::GetFieldInfo(int index , sdbus::string& name , int&  type) const 
{
    uint32_t tag = 0 ;
    if(GetFieldInfo(index , tag , type) == false)
        return false ;
    name = Tag2Name(tag) ;
    return true ;
}

bool VariantMap::GetFieldValue(int index , const Variant *& var) const 
{
    if(index < 0 || index >= size_)
        return false ;

    var = variants_ + index ;
    return true ;
}

bool VariantMap::SetBool(uint32_t tag, bool value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetBool(value) ;
}

bool VariantMap::SetUInt8(uint32_t tag, uint8_t value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetUInt8(value) ;
}

bool VariantMap::SetUInt16(uint32_t tag, uint16_t value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetUInt16(value) ;
}

bool VariantMap::SetUInt32(uint32_t tag, uint32_t value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetUInt32(value) ;
}

bool VariantMap::SetUInt64(uint32_t tag, uint64_t value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetUInt64(value) ;
}

bool VariantMap::SetInt8(uint32_t tag, int8_t value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetInt8(value) ;
}

bool VariantMap::SetInt16(uint32_t tag, int16_t value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetInt16(value) ;
}

bool VariantMap::SetInt32(uint32_t tag, int32_t value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetInt32(value) ;
}

bool VariantMap::SetInt64(uint32_t tag, int64_t value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetInt64(value) ;
}

bool VariantMap::SetFloat(uint32_t tag, float value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetFloat(value) ;
}

bool VariantMap::SetDouble(uint32_t tag, double value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetDouble(value) ;
}

bool VariantMap::SetString(uint32_t tag, const sdbus::string& value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetString(value) ;
}

bool VariantMap::SetString(uint32_t tag , const char * value) 
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetString(value) ;
}

bool VariantMap::SetRaw(uint32_t tag, const char *value, size_t size)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetRaw(value , size) ;
}

bool VariantMap::SetRaw(uint32_t tag, const sdbus::string& value)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetRaw(value) ;
}

bool VariantMap::SetMap(uint32_t tag , const VariantMap& map)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetMap(map) ;    
}

bool VariantMap::SetMap(uint32_t tag , VariantMap& map)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetMap(map) ;    
}

bool VariantMap::SetList(uint32_t tag , const VariantList& list)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetList(list) ;    
}

bool VariantMap::SetList(uint32_t tag , VariantList& list)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->SetList(list) ;    
}

bool VariantMap::AddUInt64(uint32_t tag , uint64_t val)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->AddUInt64(val) ;    
}

bool VariantMap::AddFloat(uint32_t tag , float val)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->AddFloat(val) ;    
}

bool VariantMap::AddDouble(uint32_t tag , double val)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->AddDouble(val) ;    
}

bool VariantMap::AddRaw(uint32_t tag , sdbus::string& raw)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return false ;

    return var->AddRaw(raw) ;    
}

VariantMap* VariantMap::AddMap(uint32_t tag)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return NULL ;

    VariantMap * map = new VariantMap() ;
    var->Type = Variant::VAR_MAP ;
    var->Data.map = map ;
    var->Flag = 1 ;
    return map ;
}

VariantList* VariantMap::AddList(uint32_t tag)
{
    Variant * var = AddVariant(tag) ;
    if(var == NULL)
        return NULL ;

    VariantList * list = new VariantList() ;
    var->Type = Variant::VAR_LIST ;
    var->Data.list = list ;
    var->Flag = 1 ;
    return list ;
}

Variant* VariantMap::AddVariant(uint32_t tag)
{
    if(extend() == false)
        return NULL ;

    int index = size_++ ;

    Variant* var = variants_ + index ;
    var->Clear() ;
    var->Tag = tag ;

    return var ;
}

bool VariantMap::GetBool(uint32_t tag, bool &value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetBool(value) ;
}

bool VariantMap::GetUInt8(uint32_t tag, uint8_t &value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetUInt8(value) ;
}

bool VariantMap::GetUInt16(uint32_t tag, uint16_t &value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetUInt16(value) ;
}

bool VariantMap::GetUInt32(uint32_t tag, uint32_t &value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetUInt32(value) ;
}

bool VariantMap::GetUInt64(uint32_t tag, uint64_t &value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetUInt64(value) ;
}

bool VariantMap::GetInt8(uint32_t tag, int8_t &value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetInt8(value) ;
}

bool VariantMap::GetInt16(uint32_t tag, int16_t &value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetInt16(value) ;
}

bool VariantMap::GetInt32(uint32_t tag, int32_t &value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetInt32(value) ;
}

bool VariantMap::GetInt64(uint32_t tag, int64_t &value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetInt64(value) ;
}

bool VariantMap::GetFloat(uint32_t tag, float &value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetFloat(value) ;
}

bool VariantMap::GetDouble(uint32_t tag, double &value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetDouble(value) ;
}

bool VariantMap::GetString(uint32_t tag, sdbus::string& value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetString(value) ;
}

bool VariantMap::GetRaw(uint32_t tag, sdbus::string& value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetRaw(value) ;
}

bool VariantMap::GetMap(uint32_t tag, VariantMap&value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetMap(value) ;
}

bool VariantMap::GetList(uint32_t tag , VariantList& value) const
{
    const Variant* var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->GetList(value) ;
}


bool VariantMap::RefString(uint32_t tag, const char *& value, size_t &size) const
{
    const Variant * var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->RefString(value , size) ;
}

bool VariantMap::RefString(uint32_t tag, const sdbus::string *& value) const
{
    const Variant * var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->RefString(value) ;
}

bool VariantMap::RefRaw(uint32_t tag, const void *& value, size_t &size) const
{
    const Variant * var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->RefRaw(value , size) ;
}

bool VariantMap::RefRaw(uint32_t tag, const sdbus::string *& value) const
{
    const Variant * var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->RefRaw(value) ;
}

bool VariantMap::RefMap(uint32_t tag, const VariantMap* &value) const
{
    const Variant * var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->RefMap(value) ;
}

bool VariantMap::RefList(uint32_t tag , const VariantList*& value) const
{
    const Variant * var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->RefList(value) ;
}

bool VariantMap::RefVar(uint32_t tag, const Variant* &value) const
{
    const Variant * var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    value = var ;
    return true ;
}

bool VariantMap::RefVar(uint32_t tag, Variant*& value)
{
    Variant * var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    value = var ;
    return true ;
}

bool VariantMap::RefMap(uint32_t tag, VariantMap* &value)
{
    Variant * var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->RefMap(value) ;
}

bool VariantMap::RefList(uint32_t tag , VariantList*& value)
{
    Variant * var = NULL ;
    if(get_value(tag , var) == false || var == NULL)
        return false ;

    return var->RefList(value) ;
}

bool VariantMap::extend() 
{
    if(size_ < capacity_)
        return true ;

    int cap = 0 ;
    if(variants_ == NULL)
        cap = 32 ;
    else
        cap = capacity_ + 16;

    size_t size = sizeof(Variant) * cap ;
    Variant * vars = (Variant *)::malloc(size) ;
    if(vars == NULL)
        return false ;

    ::memset(vars , 0 , size) ;

    Variant *tmp = variants_ ;

    size_t clen = sizeof(Variant) * size_ ;
    if(tmp != NULL && clen > 0)
    {
        memcpy(vars , variants_ , clen) ;
    }

    variants_ = vars ;
    capacity_ = cap ;

    if(tmp != NULL)
        ::free(tmp) ;

    return true ;
}

void VariantMap::copy(const VariantMap& map)
{
    if(this == &map)
        return ;

    Clear() ;

    if(map.Empty() == true)
        return ;

    size_t size = sizeof(Variant) * map.capacity_ ;

    variants_ = (Variant *)::malloc(size) ;
    if(variants_ == NULL)
        return ;

    ::memset(variants_ , 0 , size) ;
    int s = map.Size() ;
    for(int vidx = 0 ; vidx < s ; ++vidx)
    {
        variants_[vidx].Assign(map.variants_[vidx]) ;
    }

    /*
    if(fmaps_ == NULL)
        fmaps_ = new FieldMap() ;

    *fmaps_ = *map.fmaps_ ;
    */

    size_ = map.size_ ;
    capacity_ = map.capacity_ ;   
    dict_ = map.dict_ ;
}

bool VariantMap::get_value(uint32_t tag , const Variant*& var) const 
{
    if(variants_ == NULL)
        return false ;

    int index = 0 ;
    if(get_index(tag , index) == false || index >= size_)
        return false ;

    var = variants_ + index ;
    return true ;
}

bool VariantMap::get_value(uint32_t tag , Variant*& var) 
{
    if(variants_ == NULL)
        return false ;

    int index = 0 ;
    if(get_index(tag , index) == false || index >= size_)
        return false ;

    var = variants_ + index ;
    return true ;
}

bool VariantMap::get_index(uint32_t tag , int& index) const
{
    /*
    if(fmaps_ == NULL)
        return false ;
    return fmaps_->Get(tag , index) ;
    */

    /*
        这个算法非常有意思。我们基于这样的假设：
        在绝大多数情况下，在Map中的访问顺序和设置顺序基本相同。
        于是，我们认为，在下次get的tag，为本次tag的下一个。例如，
        set的顺序为 1027 、 1010 、 1038 、 56。那么get的顺序也是类似的。
    */
    int tidx = last_index_ ;
    int times = 0 ;
    for(; times < size_ ; ++times)
    {
        const Variant& var = variants_[tidx] ;
        if(var.Tag == tag)
        {
            index = tidx ;
            break ;
        }

        ++tidx ;
        if(tidx >= size_)
            tidx = 0 ;
    }

    if(times < size_)
    {
        last_index_ = tidx ;
        return true ;
    }

    return false ;

}

void VariantMap::init() 
{
    variants_ = NULL ;
    size_ = 0 ;
    capacity_ = 0 ;
    last_index_ = 0 ;
    dict_ = NULL ;
}

void VariantMap::Attach(VariantMap& map) 
{
    Clear() ;

    variants_ = map.variants_ ;
    size_ = map.size_ ;
    capacity_ = map.capacity_ ;
    dict_ = map.dict_ ;
    map.init() ;
}

uint32_t VariantMap::Name2Tag(const char * name) const
{
    const FieldDef& def = (dict_ == NULL) ? FieldDict::Singleton().Find(name) : dict_->Find(name) ;
    return def.Tag ;
}

const char * VariantMap::Tag2Name(uint32_t tag) const 
{
    const FieldDef& def = (dict_ == NULL) ? FieldDict::Singleton().Find(tag) : dict_->Find(tag) ;
    return def.Name.c_str() ;
}

const VariantMap VariantMap::EMPTY ;

const char * variant_type_to_desc[] = {
    "VAR_VOID"  ,
    "VAR_BOOL"  ,
    "VAR_INT8"  ,
    "VAR_UINT8"  ,
    "VAR_INT16"  ,
    "VAR_UINT16"  ,
    "VAR_INT32"  ,
    "VAR_UINT32"  ,
    "VAR_INT64"  ,
    "VAR_UINT64"  ,
    "VAR_FLOAT"  ,
    "VAR_DOUBLE"  ,
    "VAR_STRING"  ,
    "VAR_RAW"  ,
    "VAR_MAP"  ,
    "VAR_LIST"  
} ;

const char * variant_type_unknown = "VAR_UNKNOWN" ;


const char * Variant::Type2Desc(int type) 
{
    if(type < Variant::VAR_MIN || type > Variant::VAR_MAX)
        return variant_type_unknown ;
    else
        return variant_type_to_desc[type] ;
}

void Variant::inner_init() 
{
    Type = VAR_VOID ;
    Flag = 0 ;
    Tag  = 0 ;
    Data.val = 0 ;
}

Variant::Variant() 
{
    inner_init() ;
}

void Variant::Clear()
{
    if(Type == VAR_STRING || Type == VAR_RAW)
    {
        sdbus::string * str = Data.str ;
        if(str != NULL)
        {
            delete str ;
        }
    }
    else if(Type == VAR_MAP)
    {
        Map * map = Data.map ;
        if(map != NULL)
        {
            delete map ;
        }
    }
    else if(Type == VAR_LIST)
    {
        List * list = Data.list ;
        if(list != NULL)
        {
            delete list ;
        }
    }

    Data.val = 0 ;
    Flag = 0 ;
}

void Variant::Reset() 
{
    Clear() ;
    inner_init() ;
}

bool Variant::Assigned() const
{
    return (Flag & 1) != 0 ;
}

void Variant::Assigned(bool val) 
{
    if(val == true)
    {
        Flag |= 1 ;
    }
    else
    {
        Flag &= ~1 ;
    }
}

void Variant::Assign(const Variant& var)
{
    Clear() ;

    Type = var.Type ;
    Flag = var.Flag ;
    Tag  = var.Tag  ;

    memcpy(&Data , &var.Data , sizeof(VariantData)) ;

    if(Type == VAR_STRING || Type == VAR_RAW)
    {
        sdbus::string * src = var.Data.str ;
        if(src != NULL)
        {
            sdbus::string * str = new sdbus::string(*src) ;
            Data.str = str ;        
        }
    }
    else if(Type == VAR_MAP)
    {
        Map * src = var.Data.map ;
        if(src != NULL)
        {
            Map * map = new Map(*src) ;
            Data.map = map ;    
        }
    }
    else if(Type == VAR_LIST)
    {
        List * src = var.Data.list ;
        if(src != NULL)
        {
            List * list = new List(*src) ;
            Data.list = list ;
        }
    }
}

Variant::Variant(const Variant& var)
{
    inner_init() ;

    Assign(var) ;
}

Variant::Variant(int type , uint32_t tag)
{
    Type = type ;
    Flag = 0 ;
    Tag = tag ;
    Data.val = 0 ;
}

Variant::Variant(bool val)
{
    inner_init() ;

    SetBool(val) ;
}

Variant::Variant(int8_t val)
{
    inner_init() ;

    SetInt8(val) ;
}

Variant::Variant(uint8_t val)
{
    inner_init() ;

    SetUInt8(val) ;
}

Variant::Variant(int16_t val)
{
    inner_init() ;

    SetInt16(val) ;
}

Variant::Variant(uint16_t val)
{
    inner_init() ;

    SetUInt16(val) ;
}

Variant::Variant(int32_t val)
{
    inner_init() ;

    SetInt32(val) ;
}

Variant::Variant(uint32_t val)
{
    inner_init() ;

    SetUInt32(val) ;
}

Variant::Variant(int64_t val)
{
    inner_init() ;

    SetInt64(val) ;
}

Variant::Variant(uint64_t val)
{
    inner_init() ;

    SetUInt64(val) ;
}


Variant::Variant(float val)
{
    inner_init() ;

    SetFloat(val) ;
}

Variant::Variant(double val)
{
    inner_init() ;

    SetDouble(val) ;
}

Variant::Variant(const char * val)
{
    inner_init() ;

    SetString(val) ;
}

Variant::Variant(const sdbus::string& val)
{
    inner_init() ;

    SetString(val) ;
}

Variant::Variant(const void * val , int len)
{
    inner_init() ;

    SetRaw(val , len) ;
}

Variant::Variant(const Map& map)
{
    inner_init() ;

    SetMap(map) ;
}

Variant::Variant(const List& list)
{
    inner_init() ;

    SetList(list) ;
}

Variant::~Variant()
{
    Clear() ;
}

bool Variant::SetBool(bool val)
{
    Clear() ;
    Type = VAR_BOOL ;

    Data.val = 0 ;
    Data.bv = val ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetInt8(int8_t val)
{
    Clear() ;
    Type = VAR_INT8 ;

    Data.val = 0 ;
    Data.i8 = val ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetUInt8(uint8_t val)
{
    Clear() ;
    Type = VAR_UINT8 ;

    Data.val = 0 ;
    Data.u8 = val ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetInt16(int16_t val)
{
    Clear() ;
    Type = VAR_INT16 ;

    Data.val = 0 ;
    Data.i16 = val ;
    Flag = 1 ;
    return true ;

}

bool Variant::SetUInt16(uint16_t val)
{
    Clear() ;
    Type = VAR_UINT16 ;

    Data.val = 0 ;
    Data.u16 = val ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetInt32(int32_t val)
{
    Clear() ;
    Type = VAR_INT32 ;

    Data.val = 0 ;
    Data.i32 = val ;
    Flag = 1 ;
    return true ;

}

bool Variant::SetUInt32(uint32_t val)
{
    Clear() ;
    Type = VAR_UINT32 ;

    Data.val = 0 ;
    Data.u32 = val ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetInt64(int64_t val)
{
    Clear() ;
    Type = VAR_INT64 ;

    Data.val = 0 ;
    Data.i64 = val ;
    Flag = 1 ;
    return true ;

}

bool Variant::SetUInt64(uint64_t val)
{
    Clear() ;
    Type = VAR_UINT64 ;

    Data.val = 0 ;
    Data.u64 = val ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetFloat(float val)
{
    Clear() ;
    Type = VAR_FLOAT ;

    Data.val = 0 ;
    Data.f32 = val ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetDouble(double val)
{
    Clear() ;
    Type = VAR_DOUBLE ;

    Data.val = 0 ;
    Data.f64 = val ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetString(const char * val)
{
    Clear() ;
    Type = VAR_STRING ;

    if(val == NULL)
        return true ;

    Data.str = new sdbus::string(val) ;
    Flag = 1 ;

    return true ;
}

bool Variant::SetString(const sdbus::string& val)
{
    return SetString(val.c_str()) ;
}
/*
bool Variant::SetString(const std::string& val) 
{
    Clear() ;
    Type = VAR_STRING ;

    sdbus::string * str = new sdbus::string() ;
    //str->attach(val) ;
    str->assign(val.c_str() , val.length()) ;

    Data.str = str ;
    Flag = 1 ;
    return true ;
}
*/
bool Variant::SetRaw(const void * val , int len)
{
    Clear() ;
    Type = VAR_RAW ;

    if(val == NULL || len <= 0)
        return true ;

    Data.raw = new sdbus::string((const char *)val , len) ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetRaw(const sdbus::string& val) 
{
    Clear() ;
    Type = VAR_RAW ;

    sdbus::string * raw = new sdbus::string() ;
    raw->assign(val.data() , val.length()) ;

    Data.raw = raw ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetMap(const Map& map)
{
    Clear() ;
    Type = VAR_MAP ;

    Data.map = new Map(map) ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetMap(Map& map) 
{
    Clear() ;
    Type = VAR_MAP ;

    VariantMap * var = new VariantMap() ;
    if(var== NULL)
        return false ;

    var->Attach(map) ;

    Data.map = var ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetList(const List& list)
{    
    Clear() ;
    Type = VAR_LIST ;

    Data.list = new VariantList(list) ;
    Flag = 1 ;
    return true ;
}

bool Variant::SetList(List& list) 
{
    Clear() ;
    Type = VAR_LIST ;

    VariantList * var = new VariantList() ;
    if(var == NULL)
        return false ;
    var->Attach(list) ;

    Data.list = var ;
    Flag = 1 ;
    return true ;
}

bool Variant::GetBool(bool& val) const
{
    int64_t i64 = 0 ;
    if(GetInt64(i64) == false)
        return false ;
    val = (i64 != 0) ;
    return true ;
}

bool Variant::GetInt8(int8_t& val) const
{
    int64_t i64 = 0 ;
    if(GetInt64(i64) == false)
        return false ;
    
    val = (int8_t)i64 ;
    return true ;
}

bool Variant::GetUInt8(uint8_t& val) const
{
    int64_t i64 = 0 ;
    if(GetInt64(i64) == false)
        return false ;
    
    val = (uint8_t)i64 ;
    return true ; 
}

bool Variant::GetInt16(int16_t& val) const
{
    int64_t i64 = 0 ;
    if(GetInt64(i64) == false)
        return false ;
    
    val = (int16_t)i64 ;
    return true ; 
}

bool Variant::GetUInt16(uint16_t& val) const
{
    int64_t i64 = 0 ;
    if(GetInt64(i64) == false)
        return false ;
    
    val = (uint16_t)i64 ;
    return true ; 
}

bool Variant::GetInt32(int32_t& val) const
{
    int64_t i64 = 0 ;
    if(GetInt64(i64) == false)
        return false ;
    
    val = (int32_t)i64 ;
    return true ; 
}

bool Variant::GetUInt32(uint32_t& val) const
{
    int64_t i64 = 0 ;
    if(GetInt64(i64) == false)
        return false ;
    
    val = (uint32_t)i64 ;
    return true ; 
}

bool Variant::GetInt64(int64_t& val) const
{
     if(Type == VAR_VOID || Flag == 0)
        return false ;

    if(Type == VAR_BOOL)
    {
        val = Data.bv ? 1 : 0 ;
    }
    else if(Type == VAR_INT8)
    {
        val = Data.i8 ;
    }
    else if(Type == VAR_UINT8)
    {
        val = Data.u8 ;
    }
    else if(Type == VAR_INT16)
    {
        val = Data.i16 ;
    }
    else if(Type == VAR_UINT16)
    {
        val = Data.u16 ;
    }
    else if(Type == VAR_INT32)
    {
        val = Data.i32 ;
    }
    else if(Type == VAR_UINT32)
    {
        val = Data.u32 ;
    }
    else if(Type == VAR_INT64)
    {
        val = Data.i64 ;
    }
    else if(Type == VAR_UINT64)
    {
        val = Data.u64 ;
    }
    else if(Type == VAR_FLOAT || Type == VAR_DOUBLE)
    {
        double f64 = 0 ;
        if(Type == VAR_FLOAT)
            f64 = Data.f32 ;
        else
            f64 = Data.f64 ;

        if(f64 < 0)
            f64 -= 0.5 ;
        else
            f64 += 0.5 ;
        val = (int64_t)f64 ;
    }
    else if(Type == VAR_STRING || Type == VAR_RAW)
    {
        const sdbus::string * str = Data.str ;
        if(check_string(str) == false)
            return false ;

        val = atoi64(str->c_str()) ;
    }
    else
    {
        return false ;
    }

    return true ;
}

bool Variant::GetUInt64(uint64_t& val) const
{
    int64_t i64 = 0 ;
    if(GetInt64(i64) == false)
        return false ;

    val = (uint64_t)i64 ;
    return true ;
}

bool Variant::GetFloat(float& val) const
{
    double f64 = 0 ;
    if(GetDouble(f64) == false)
        return false ;
    val = (float)f64 ;
    return true ;
}

bool Variant::GetDouble(double& val) const
{
    if(Type == VAR_VOID || Flag == 0)
        return false ;

    if(Type == VAR_INT8)
    {
        val = Data.i8 ;
    }
    else if(Type == VAR_UINT8)
    {
        val = Data.u8 ;
    }
    else if(Type == VAR_INT16)
    {
        val = Data.i16 ;
    }
    else if(Type == VAR_UINT16)
    {
        val = Data.u16 ;
    }
    else if(Type == VAR_INT32)
    {
        val = Data.i32 ;
    }
    else if(Type == VAR_UINT32)
    {
        val = Data.u32 ;
    }
    else if(Type == VAR_INT64)
    {
        val = (double)Data.i64 ;
    }
    else if(Type == VAR_UINT64)
    {
        val = (double)Data.u64 ;
    }
    else if(Type == VAR_FLOAT)
    {
        val = Data.f32 ;
    }
    else if(Type == VAR_DOUBLE)
    {
        val = Data.f64 ;
    }
    else if(Type == VAR_STRING || Type == VAR_RAW)
    {
        const sdbus::string * str = Data.str ;
        if(check_string(str) == false)
            return false ;

        val = atof(str->c_str()) ;
    }
    else
    {
        return false ;
    }

    return true ;
}

int trim_float(char * str , int slen)
{
    //1、先搜索最后一个非0的字节
    int idx = slen -1 ;
    while(idx >= 0)
    {
        if(str[idx] == '0')
            --idx ;
        else
            break ;
    }

    //2、再搜索小数点
    int dot = idx ;
    while(dot >= 0)
    {
        if(str[dot] == '.')
            break ;
        else
            --dot ;
    }

    if(idx < 0)
    {
        //都是0        
        str[0] = '0' ;
        str[1] = '\0' ;
        return 1 ;
    }

    if(dot < 0)
    {
        //没有小数点，idx无效
        return slen ;
    }

    if(idx != dot)
    {
        ++idx ;
    }

    str[idx] = '\0' ;
    return idx ;
}

int float2str(char * str , int size , float val)
{
    int slen =::sprintf(str , "%0.4f" , val) ;
    return trim_float(str , slen) ;
}

int double2str(char * str , int size , double val)
{
    int slen =::sprintf(str , "%0.6f" , val) ;
    return trim_float(str , slen) ;
}

bool Variant::GetString(sdbus::string& val) const
{
    if(Type == VAR_VOID || Flag == 0)
        return false ;

    if(Type == VAR_STRING || Type == VAR_RAW)
    {
        if(Data.str == NULL)
            return false ;

        val = *(Data.str) ;
        return true ;
    }

    char str[256] ;
    int slen = 0 ;
    switch(Type)
    {
    case VAR_BOOL :
    case VAR_INT8 :
        slen = ::sprintf(str , "%hhd" , Data.i8) ;
        break ;
    case VAR_UINT8 :
        slen = ::sprintf(str , "%hhu" , Data.u8) ;
        break ;
    case VAR_INT16 :
        slen = ::sprintf(str , "%hd" , Data.i16) ; 
        break ;
    case VAR_UINT16 :
        slen = ::sprintf(str , "%hu" , Data.u16) ; 
        break ;
    case VAR_INT32 :
        slen = ::sprintf(str , "%d" , Data.i32) ; 
        break ;
    case VAR_UINT32 :
        slen = ::sprintf(str , "%u" , Data.u32) ; 
        break ;
    case VAR_INT64 :
        slen = ::sprintf(str , I64FMT , Data.i64) ; 

        break ;
    case VAR_UINT64 :
        slen = ::sprintf(str , U64FMT , Data.u64) ; 
        break ;		
    case VAR_FLOAT :
        slen = float2str(str , 256 , Data.f32) ;
        break ;
    case VAR_DOUBLE :
        slen = double2str(str , 256 , Data.f64) ;
        break ;
    default:
        return false ;        
    }

    val = str ;

    return true ;
}

bool Variant::GetRaw(void * val , size_t & len) const
{
    if(val == NULL)
        return false ;

    if((Type != VAR_RAW && Type != VAR_STRING) || Flag == 0)
        return false ;

    const sdbus::string * raw = Data.raw ;
    if(raw == NULL)
        return false ;

    if(raw->length() > len)
        return false ;

    len = (int)raw->length() ;
    ::memcpy(val , raw->c_str() , len) ;    
    return true ;
}


bool Variant::GetRaw(sdbus::string& val) const
{
    if((Type != VAR_RAW && Type != VAR_STRING)|| Flag == 0)
        return false ;

    const sdbus::string * raw = Data.raw ;
    if(raw == NULL)
        return false ;

    val = *raw ;
    return true ;
}


bool Variant::GetMap(Map& map) const
{
    const VariantMap * cmap = NULL ;
    if(RefMap(cmap) == false || cmap == NULL)
        return false ;

    map = *cmap ;
    return true ;
}

bool Variant::GetList(List& list) const
{
    const VariantList * clist = NULL ;
    if(RefList(clist) == false || clist == NULL)
        return false ;

    list = *clist ;
    return true ;
}

bool Variant::RefString(const char *& str , size_t& len) const
{
    if(Type != VAR_STRING || Flag == 0 || Data.str == NULL)
        return false ;

    const sdbus::string * src = Data.str ;

    str = src->c_str() ;
    len = src->length() ;    
    return true ;
}

bool Variant::RefString(const sdbus::string *& str) const 
{
    if(Type != VAR_STRING || Flag == 0 || Data.str == NULL)
        return false ;

    str = Data.str ;
    return true ;
}

bool Variant::RefString(sdbus::string *& str) 
{
    if(Type != VAR_STRING || Flag == 0 || Data.str == NULL)
        return false ;

    str = Data.str ;
    return true ;
}

bool Variant::RefRaw(const void *& val , size_t& len) const
{
    if(Type != VAR_RAW || Flag == 0 || Data.str == NULL)
        return false ;

    const sdbus::string * src = Data.raw ;

    val = src->c_str() ;
    len = src->length() ;    
    return true ;
}

bool Variant::RefRaw(const sdbus::string* & raw) const
{
    if(Type != VAR_RAW || Flag == 0 || Data.raw == NULL)
        return false ;

    raw = Data.raw ;
    return true ;
}

bool Variant::RefRaw(sdbus::string* & raw)
{
    if(Type != VAR_RAW || Flag == 0 || Data.raw == NULL)
        return false ;

    raw = Data.raw ;
    return true ;
}

bool Variant::RefMap(const Map *& map) const
{
    if(Flag == 0)
        return false ;

    if(Type != VAR_MAP)
        return false ;

    map = Data.map ;
    return true ;
}

bool Variant::RefList(const List *& list) const
{
    if(Flag == 0)
        return false ;

    to_list() ;

    if(Type != VAR_LIST)
        return false ;

    list = Data.list ;
    return true ;
}

bool Variant::RefMap(Map *& map) 
{
    if(Flag == 0)
        return false ;

    if(Type != VAR_MAP)
        return false ;

    map = Data.map ;
    return true ;
}

bool Variant::RefList(List *& list) 
{
    if(Flag == 0)
        return false ;

    to_list() ;

    if(Type != VAR_LIST)
        return false ;

    list = Data.list ;
    return true ;
}

Variant& Variant::operator=(bool val)
{
    SetBool(val) ;
    return (*this) ;
}

Variant& Variant::operator=(int8_t val)
{
    SetInt8(val) ;
    return (*this) ;
}

Variant& Variant::operator=(uint8_t val)
{
    SetUInt8(val) ;
    return (*this) ;
}

Variant& Variant::operator=(int16_t val)
{
    SetInt16(val) ;
    return (*this) ;
}

Variant& Variant::operator=(uint16_t val)
{
    SetUInt16(val) ;
    return (*this) ;
}

Variant& Variant::operator=(int32_t val)
{
    SetInt32(val) ;
    return (*this) ;
}

Variant& Variant::operator=(uint32_t val)
{
    SetUInt32(val) ;
    return (*this) ;
}

Variant& Variant::operator=(int64_t val)
{
    SetInt64(val) ;
    return (*this) ;
}

Variant& Variant::operator=(uint64_t val)
{
    SetUInt64(val) ;
    return (*this) ;
}

Variant& Variant::operator=(float val)
{
    SetFloat(val) ;
    return (*this) ;
}

Variant& Variant::operator=(double val)
{
    SetDouble(val) ;
    return (*this) ;
}


Variant& Variant::operator=(const char * val)
{
    SetString(val) ;
    return (*this) ;
}

Variant& Variant::operator=(const sdbus::string& val)
{
    SetString(val) ;
    return (*this) ;
}

Variant& Variant::operator=(const Map& map)
{
    SetMap(map) ;
    return (*this) ;
}

Variant& Variant::operator=(const List& list)
{
    SetList(list);
    return (*this) ;
}


Variant& Variant::operator=(const Variant& var)
{
    Assign(var) ;
    return (*this) ;
}

Variant::operator bool() const
{
    if(Type == VAR_BOOL)
        return Data.bv ;

    bool val = false ;
    if(GetBool(val) == false)
        return false ;
    return  val ;
}

Variant::operator int8_t() const
{
    if(Type == VAR_INT8)
        return Data.i8 ;

    int8_t val = 0 ;
    if(GetInt8(val) == false)
        return 0 ;
    return val ;
}

Variant::operator uint8_t() const
{
    if(Type == VAR_UINT8)
        return Data.u8 ;

    uint8_t val = 0 ;
    if(GetUInt8(val) == false)
        return 0 ;

    return val ;
}

Variant::operator int16_t() const
{
    if(Type == VAR_INT16)
        return Data.i16 ;

    int16_t val = 0 ;
    if(GetInt16(val) == false)
        return 0 ;
    return val ;
}

Variant::operator uint16_t() const
{
    if(Type == VAR_UINT16)
        return Data.u16 ;

    uint16_t val = 0 ;
    if(GetUInt16(val) == false)
        return 0 ;
    return val ;
}

Variant::operator int32_t() const
{
    if(Type == VAR_INT32)
        return Data.i32 ;

    int32_t val = 0 ;
    if(GetInt32(val) == false)
        return 0 ;
    return val ;
}

Variant::operator uint32_t() const
{
    if(Type == VAR_UINT32)
        return Data.u32 ;

    uint32_t val = 0 ;
    if(GetUInt32(val) == false)
        return 0 ;
    return val ;
}

Variant::operator int64_t() const
{
    if(Type == VAR_INT64)
        return Data.i64 ;

    int64_t val = 0 ;
    if(GetInt64(val) == false)
        return 0 ;
    return val ;
}

Variant::operator uint64_t() const
{
    if(Type = VAR_UINT64)
        return Data.u64 ;

    uint64_t val = 0 ;
    if(GetUInt64(val) == false)
        return 0 ;
    return val ;
}

Variant::operator float() const
{
    if(Type == VAR_FLOAT)
        return Data.f32 ;

    float val = 0 ;
    if(GetFloat(val) == false)
        return 0 ;
    return val ;
}

Variant::operator double() const
{
    if(Type == VAR_DOUBLE)
        return Data.f64 ;

    double val = 0 ;
    if(GetDouble(val) == false)
        return 0 ;
    return val ;
}

Variant::operator const sdbus::string & () const 
{
    const sdbus::string * str = Data.str ;

    if(Type != VAR_STRING || Flag == 0 || str == NULL)
        return sdbus::string::EMPTY ;
    
    return *str ;
}

Variant::operator const Variant::Map& () const
{
    if(Type != VAR_MAP || Data.map == NULL)
        return VariantMap::EMPTY ;

    const Variant::Map * map = Data.map ;

    return *map ;
}

Variant::operator const Variant::List& () const
{
    if(Type != VAR_LIST || Data.list == NULL)
        return VariantList::EMPTY ;

    const Variant::List * list = Data.list ;

    return *list ;
}

bool Variant::ToBool() const 
{
    bool val = false ;
    if(GetBool(val) == false)
        return false ;
    else
        return val ;
}

int32_t Variant::ToInt32() const
{
    int32_t val = 0 ;
    if(GetInt32(val) == false)
        return 0 ;
    else
        return val ;
}

int64_t Variant::ToInt64() const
{
    int64_t val = 0 ;
    if(GetInt64(val) == false)
        return 0 ;
    else
        return val ;
}

uint32_t Variant::ToUInt32() const
{
    uint32_t val =0 ;
    if(GetUInt32(val) == false)
        return 0 ;
    else
        return val ;
}

uint64_t Variant::ToUInt64() const
{
    uint64_t val = 0 ;
    if(GetUInt64(val) == false)
        return 0 ;
    else
        return val ;
}

float Variant::ToFloat() const
{
    float val = 0 ;
    if(GetFloat(val) == false)
        return 0 ;
    else
        return val ;
}

double Variant::ToDouble() const
{
    double val = 0 ;
    if(GetDouble(val) == false)
        return 0 ;
    else
        return val ;
}

const sdbus::string Variant::ToString() const
{
    sdbus::string str ;
    if(GetString(str) == false)
        return sdbus::string::EMPTY ;
    else
        return str ;
}

const Variant::Map& Variant::ToMap() const
{
    if(Type != VAR_MAP || Assigned() == false || Data.map == NULL)
        return VariantMap::EMPTY ;
    const VariantMap * map = Data.map ;
    return *map ;
}

const Variant::List& Variant::ToList() const
{
    if(Type != VAR_LIST || Assigned() == false || Data.list == NULL)
        return VariantList::EMPTY ;
    const VariantList * list = Data.list ;
    return *list ;
}
/*
bool Variant::CheckInteger() const 
{
    if(Type >= VAR_BOOL && Type <= VAR_UINT64)
        return true ;
    else
        return false ;
}
*/
/*
bool Variant::CheckDecimal() const
{
    if(Type == VAR_FLOAT || Type == VAR_DOUBLE)
        return true ;
    else
        return false ;
}
*/

void Variant::to_list() const
{
    if(Type == VAR_LIST)
        return ;

    VariantList * list = new VariantList() ;
    list->Tag(Tag) ;

    Variant * var = list->AddVariant() ;
    var->Tag = Tag ;
    var->Type = Type ;
    var->Flag = Flag ;
    var->Data.val = Data.val ;

    Type = VAR_LIST ;
    Data.list = list ;
}

bool Variant::AddUInt64(uint64_t val)
{
    if(Type == VAR_VOID || Flag == 0)
    {
        //没有赋值过，那么直接赋值
        return SetUInt64(val) ;
    }

    /*
        将原有的值转换为list
    */
    to_list() ;
     
    VariantList * list = Data.list ;
    //添加新元素
    Variant *var = list->AddVariant() ;

    var->SetUInt64(val) ;
    var->Tag = Tag ;    

    return true ;
}

bool Variant::AddFloat(float val)
{
    if(Type == VAR_VOID || Flag == 0)
    {
        //没有赋值过，那么直接赋值
        return SetFloat(val) ;
    }

    /*
        将原有的值转换为list
    */
    to_list() ;
     
    VariantList * list = Data.list ;
    //添加新元素
    Variant *var = list->AddVariant() ;

    var->SetFloat(val) ;
    var->Tag = Tag ;    

    return true ;
}

bool Variant::AddDouble(double val)
{
    if(Type == VAR_VOID || Flag == 0)
    {
        //没有赋值过，那么直接赋值
        return SetDouble(val) ;
    }

    /*
        将原有的值转换为list
    */
    to_list() ;
     
    VariantList * list = Data.list ;
    //添加新元素
    Variant *var = list->AddVariant() ;

    var->SetDouble(val) ;
    var->Tag = Tag ;  

    return true ;
}

bool Variant::AddRaw(sdbus::string& raw)
{
    if(Type == VAR_VOID || Flag == 0)
    {
        //没有赋值过，那么直接赋值
        return SetRaw(raw) ;
    }

    /*
        将原有的值转换为list
    */
    to_list() ;
     
    VariantList * list = Data.list ;
    //添加新元素
    Variant *var = list->AddVariant() ;

    var->SetRaw(raw) ;
    var->Tag = Tag ;    

    return true ;
}

bool Variant::check_string(const sdbus::string * str) const
{
    if(str == NULL)
        return false ;

    size_t length = str->length() ;
    size_t capacity = str->capacity() ;
    const char * data = str->data() ;

    if(length >= capacity && length <= 0)
        return false ;

    for(int idx = (int)length ; idx >= 0 ; --idx)
    {
        if(data[idx] == '\0')
            return true ;
    }

    return false ;
}

const Variant Variant::EMPTY ;

}
