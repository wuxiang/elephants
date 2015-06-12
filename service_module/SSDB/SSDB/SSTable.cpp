#include "SSTable.h"
#include "SSType.h"
#include "SSMyLog.h"
#include "SSSQLCreator.h"
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <fstream>
#include <time.h>
#include <bitset>
#include <sstream>
#include <boost/algorithm/string.hpp>
using boost::scoped_ptr;
using boost::lexical_cast;
using std::vector;
using std::stringstream;
using std::make_pair;
using boost::to_lower;
using boost::to_lower_copy;

namespace ss {
#define SQLFIELD(x) ("`"+x+"`")
#define FROM_UNIXTIME(x) ("FROM_UNIXTIME(" + x + ")")
#define SQLSTR(x) ("\'" + x + "\'")


const string VER = "<0.01>";
const SSTable::row_type SSTable::ROW_NULL;
const string SSTable::FILE = "<FILE>";

class ss_equal_to
{
public:
    template<typename T>
    bool operator()(T &a, T &b) const
    {
        return a == b;
    }

    bool operator()(char *a, char *b) const
    {
        return string(a) == string(b);
    }
};
class ss_not_equal_to
{
public:
    template<typename T>
    bool operator()(T &a, T &b) const
    {
        return a != b;
    }

    bool operator()(char *a, char *b) const
    {
        return string(a) != string(b);
    }
};

class ss_less
{
public:
    template<typename T>
    bool operator()(T &a, T &b) const
    {
        return a < b;
    }

    bool operator()(char *a, char *b) const
    {
        return string(a) < string(b);
    }
};
class ss_less_equal
{
public:
    template<typename T>
    bool operator()(T &a, T &b) const
    {
        return a <= b;
    }

    bool operator()(char *a, char *b) const
    {
        return string(a) <= string(b);
    }
};
class ss_greater
{
public:
    template<typename T>
    bool operator()(T &a, T &b) const
    {
        return a > b;
    }

    bool operator()(char *a, char *b) const
    {
        return string(a) > string(b);
    }
};
class ss_greater_equal
{
public:
    template<typename T>
    bool operator()(T &a, T &b) const
    {
        return a >= b;
    }

    bool operator()(char *a, char *b) const
    {
        return string(a) >= string(b);
    }
};

SSTable::SSTable():m_init(false),m_errcode(0),m_field_name_to_lower(false),m_row_mapping_array_cache(NULL),m_insert_and_modify_id(0),m_insert_mode(0)
{
}

SSTable::SSTable(const string &param, const string& sqlstr):m_init(false),m_errcode(0),m_field_name_to_lower(false),m_row_mapping_array_cache(NULL),m_insert_and_modify_id(0),m_insert_mode(0)
{
    load(param, sqlstr);
}

SSTable::~SSTable()
{
    if(NULL != m_row_mapping_array_cache){
        delete m_row_mapping_array_cache;
        m_row_mapping_array_cache = NULL;
    }
}
SSTable::row_type::row_type():m_table(NULL), m_new_line(false)
{
}

SSTable::row_type::row_type(const ss::SSTable *result, const ss::db_row &row):
                                                                    m_table(const_cast<ss::SSTable *>(result)),
                                                                    m_row(row),
                                                                    m_new_line(false)
{
}

bool
SSTable::row_type::prev() const
{
    if(m_table == NULL){
        return false;
    }

    const ss::db_row *row = m_table->m_result.prev(&m_row);
    if(row == NULL){
        m_row.clear();
        return false;
    }
    else{
        m_row = *row;
        return true;
    }
}

bool
SSTable::row_type::next() const
{
    if(m_table == NULL){
        return false;
    }

    const ss::db_row *row = m_table->m_result.next(&m_row);
    if(row == NULL){
        m_row.clear();
        return false;
    }
    else{
        m_row = *row;
        return true;
    }
}

ss::field *
SSTable::row_type::get_field(ss::db_row *row, int cnt) const
{
    char *buf = row->buffer();
    if(NULL == buf || 0 == row->length() || cnt < 0){
        return NULL;
    }
    int offset = cnt * sizeof(ss::field);
    return reinterpret_cast<ss::field *>(buf + offset);
}

ss::field *
SSTable::row_type::get_field(ss::db_row *row, const string &name) const
{
    int cnt;
    if(m_table->m_field_name_to_lower){
        cnt = m_table->m_result.get_lower_case_header().find(to_lower_copy(name));
    }
    else{
        cnt = m_table->m_result.get_header().find(name);
    }
    return get_field(row, cnt);
}

const ss::field_def &
SSTable::row_type::get_field_def(const string &name) const
{
    if(m_table->m_field_name_to_lower){
        return m_table->m_result.get_lower_case_header().fields(to_lower_copy(name));
    }
    else{
        return m_table->m_result.get_header().fields(name);
    }
}

int
SSTable::row_type::size() const
{
    return static_cast<int>(m_table->m_result.row_count());
}

bool
SSTable::row_type::is_modified(const string &name) const
{
    ss::field *field = get_field(&m_row, name);
    if(NULL == field){
        return false;
    }
    return ss::get_bit_mask(field->holder()->flag, 3);
}

void
SSTable::row_type::set_null(const string &name)
{
    ss::field *field = get_field(&m_row, name);
    if(NULL == field){
        return;
    }
    bool assigned = field->assigned();
    field->clear();

    if(m_new_line){
        //return
    }
    else if(assigned){
        //set modified flag
        set_bit_mask(field->holder()->flag, 3, true);
        // add to modify list
        m_table->add_insert_and_modify_block(m_row.get_block());
    }
    return;
}

void
SSTable::row_type::del()
{
    ss::field *field = get_field(&m_row, 0);
    if(NULL == field){
        return;
    }
    //set delete flag
    ss::set_bit_mask(field->holder()->flag, 5, true);
    m_table->m_delete_list.insert(m_row.get_block());
}

void
SSTable::row_type::clear()
{
    for(int i = 0; i < this->get_field_count(); i++){
        set_null(this->get_field_name(i));
    }
}

const ss::field*
SSTable::row_type::get_field_by_name(const string &name) const
{
    return get_field(&m_row, name);
}

bool
SSTable::row_type::count(const string &name) const
{
    return m_table->count(name);
}

bool
SSTable::row_type::assigned(const string &name) const
{
    ss::field *field = get_field(&m_row, name);
    if(NULL == field){
        return false;
    }

    return field->assigned();
}

ss::field &
SSTable::row_type::operator[](const string &name)
{
    ss::field *field = get_field(&m_row, name);
    if(NULL == field){
        return ss::field::FIELD_NULL;
    }
    return *field;
}

bool
SSTable::row_type::operator==(const SSTable::row_type &rhs) const
{
    return (m_table == rhs.m_table && m_row.buffer() == rhs.m_row.buffer());
}

bool
SSTable::row_type::operator!=(const SSTable::row_type &rhs) const
{
    return !(*this == rhs);
}

const string
SSTable::row_type::get_string(const string &name) const
{
    string retstr;
    retstr.clear();
    ss::field *field = get_field(&m_row, name);
    if(NULL == field){
        return retstr;
    }
    if(false == field->assigned()){
        return retstr;
    }
    const ss::field_def &def = get_field_def(name);
    const ss::__field_holder *holder = field->holder();
    try{
        switch(field->type())
        {
        case ss::SSTYPE_INT8 : 
            retstr = lexical_cast<string>(static_cast<int>(holder->data.i8)) ;
            break ;
        case ss::SSTYPE_INT16 : 
            retstr = lexical_cast<string>(holder->data.i16) ;
            break ;
        case ss::SSTYPE_INT32 : 
            retstr = lexical_cast<string>(holder->data.i32) ;
            break ;
        case ss::SSTYPE_INT64 : 
            retstr = lexical_cast<string>(holder->data.i64) ;
            break ;
        case ss::SSTYPE_FLOAT : 
            retstr = round_to_string(holder->data.f32, def.decimals) ;
            break ;
        case ss::SSTYPE_DOUBLE : 
            retstr = round_to_string(holder->data.f64, def.decimals) ;
            break ;
        case ss::SSTYPE_STRING :
            retstr = holder->data.str;
            break ;
        case ss::SSTYPE_RAW :
            retstr = string((char *)holder->data.raw,holder->length);
            break ;
        case ss::SSTYPE_T32 :
            {
                unsigned long length =  def.length;
                switch (length)
                {
                case 4:	//year(YYYY)
                    retstr = lexical_cast<string>(holder->data.t32);
                    break;
                case 10://date(YYYY-MM-DD)
                    retstr = t32_to_string(holder->data.t32) ;
                    retstr.erase(retstr.begin() + length, retstr.end());
                    break;
                case 8://time(HH:MM:SS)
                    char time[9];
                    int h,m,s;
                    h = static_cast<int>(holder->data.t32 / 3600);
                    m = static_cast<int>((holder->data.t32 % 3600) / 60);
                    s = static_cast<int>(holder->data.t32 % 60);
                    sprintf(time, "%02d:%02d:%02d",h,m,s);
                    retstr = time;
                    break;
                default:
                    retstr = t32_to_string(holder->data.t32) ;
                    break;
                }
                break ;
            }
        case ss::SSTYPE_T64 :
            retstr = t32_to_string(holder->data.t64 / 1000) ;
            break ;
        case ss::SSTYPE_BIT64 :
            retstr = bit64_to_string(holder->data.u64);
            retstr.erase(0, retstr.size() -  def.length);
            break ;
        default:
            break ;
        }
    }
    catch(boost::bad_lexical_cast&){
        retstr.clear();
    }
    return retstr;
}

void
SSTable::row_type::set_string(const string &name, const string &value)
{
    ss::field *field = get_field(&m_row, name);
    if(NULL == field){
        return;
    }
    ss::__field_holder *holder = field->holder();
    uint64 oriData = holder->data.val;
    bool modified = false;
    bool assigned = true;
    const ss::field_def &def = get_field_def(name);
    try{
        switch(field->type())
        {
        case ss::SSTYPE_INT8 : 
            holder->data.i8 = static_cast<int8>(lexical_cast<int16>(value)) ;
            break ;
        case ss::SSTYPE_INT16 : 
            holder->data.i16 = lexical_cast<int16>(value) ;
            break ;
        case ss::SSTYPE_INT32 : 
            holder->data.i32 = lexical_cast<int32>(value) ;
            break ;
        case ss::SSTYPE_INT64 : 
            holder->data.i64 = lexical_cast<int64>(value) ;
            break ;
        case ss::SSTYPE_FLOAT : 
            {
                unsigned int decimals = def.decimals;
                float fval= round(name,lexical_cast<float>(value), decimals);
                if(!double_equal(holder->data.f32, fval, decimals)){
                    holder->data.f32 = fval;
                }
            }
            break ;
        case ss::SSTYPE_DOUBLE :
            {
                unsigned int decimals = def.decimals;
                double dval= round(name,lexical_cast<double>(value), decimals);
                if(!double_equal(holder->data.f64, dval, decimals)){
                    holder->data.f64 = dval;
                }
            }
            break ;
        case ss::SSTYPE_STRING :
            {
                fieldstr *str = reinterpret_cast<fieldstr *>(holder);
#if defined(OCI_DRIVER)
                if (value.empty() && "oci" == m_table->m_type && !str->assigned()) {
                    return;
                }
#endif
                const string &rhs = value;
                if(!str->assigned() || string(str->c_str()) != rhs){
                    *str = rhs.c_str();
                    modified = true;
                }
                break ;
            }
        case ss::SSTYPE_RAW :
            {
                fieldraw *raw = reinterpret_cast<fieldraw*>(holder);
                const string &rhs = value;
                if(!raw->assigned() || string((char*)holder->data.raw,holder->length) != rhs){
                    free(holder->data.raw);
                    if((holder->data.raw = (char *)malloc(rhs.size())) != NULL){
                        memcpy(holder->data.raw, rhs.c_str(), rhs.size());
                        holder->length = rhs.size();
                        modified = true;
                    }
                }
                break ;
            }
        case ss::SSTYPE_T32 :
            {
                unsigned long length =  def.length;
                switch (length)
                {
                case 4:	//year(YYYY)
                    holder->data.t32 = lexical_cast<time_t>(value);
                    break;
                case 10://date(YYYY-MM-DD)
                    holder->data.t32 = string_to_t32(value) ;
                    break;
                case 8://time(HH:MM:SS)
                    int h,m,s;
                    sscanf(value.c_str(), "%02d:%02d:%02d",&h,&m,&s);
                    holder->data.t32 = h * 3600 + m * 60 + s;
                    break;
                default:
                    holder->data.t32 = string_to_t32(value) ;
                    break;
                }
                break ;
            }
        case ss::SSTYPE_T64 :
            holder->data.t64 = 0;
            holder->data.t64 = static_cast<int64>(string_to_t32(value)) * 1000;
            break ;
        case ss::SSTYPE_BIT64 :
            {
                unsigned long length = def.length;
                string temp = value;
                if("true" == value){
                    temp = "1";
                }
                else if("false" == value){
                    temp = "0";
                }
                if(temp.size() > length){
                    temp.erase(0, temp.size() - length);
                }
                try{
                    holder->data.u64 = string_to_bit64(temp) ;
                }
                catch(...){
                    set_null(name);
                }
                break;
            }
        default:
            assigned = false;
            break ;
        }
    }
    catch(boost::bad_lexical_cast&){
        // use sscanf instead of lexical_cast to convert again
        if(field->type() == ss::SSTYPE_DOUBLE){
            unsigned int decimals = def.decimals;
            double dval = 0.0;
            int error = sscanf(value.c_str(), "%lf",&dval);
            if(-1 == error){
                set_null(name);
                return;
            }
            if(!double_equal(holder->data.f64, dval, decimals)){
                holder->data.f64 = dval;
            }
        }
        else if(field->type() == ss::SSTYPE_FLOAT){
            unsigned int decimals = def.decimals;
            float fval = 0.0;
            int error = sscanf(value.c_str(), "%f",&fval);
            if(-1 == error){
                set_null(name);
                return;
            }
            if(!double_equal(holder->data.f32, fval, decimals)){
                holder->data.f32 = fval;
            }
        }
        else{
            set_null(name);
            return;
        }
    }
    if(assigned != field->assigned()){
        modified = true;
    }
    if(assigned){
        //set assigned flag
        set_bit_mask(holder->flag, 0, true);
    }

    if(m_new_line){
        //return
    }
    else if(oriData != holder->data.val || true == modified){
        //set modified flag
        set_bit_mask(field->holder()->flag, 3, true);
        // add to modify list
        m_table->add_insert_and_modify_block(m_row.get_block());
    }
    return;
}

bool
SSTable::row_type::double_equal(double d1, double d2, unsigned int decimal) const
{
    static const double FIX_FLOAT_DELTA[DECIMAL_DOUBLE + 1]={
        0.9,
        0.09,
        0.009,
        0.0009,
        0.00009,
        0.000009,
        0.0000009,
        0.00000009,
        0.000000009,
        0.0000000009,
        0.00000000009,
        0.000000000009,
        0.0000000000009,
        0.00000000000009,
        0.000000000000009,
//         0.0000000000000009
    };
    decimal = (decimal > DECIMAL_DOUBLE ? DECIMAL_DOUBLE : decimal);
    double delta = d1 - d2;
    if(delta > -FIX_FLOAT_DELTA[decimal] && delta < FIX_FLOAT_DELTA[decimal]){
        return true;
    }
    else{
        return false;
    }
}

string
SSTable::row_type::round_to_string(double value,unsigned int decimals) const
{
    decimals = (decimals > DECIMAL_DOUBLE ? DECIMAL_DOUBLE : decimals);
    stringstream ss;
    ss.precision(decimals);
    ss.setf(std::ios::fixed, std::ios::floatfield);
    ss << value;
    string result = ss.str();
    while(decimals > 0 && '0' == (*result.rbegin())){
        result.erase(result.end() - 1);
    }
    if('.' == (*result.rbegin())){
        result.erase(result.end() - 1);
    }
    return result;
}

string
SSTable::row_type::t32_to_string(time_t time) const
{
    if(-1 == time){
        return "0000-00-00 00:00:00";
    }
    struct tm timeinfo ;

#if defined(WIN32) || defined(_WINDOWS) || defined(MINGW) || defined(MINGW32)
    localtime_s(&timeinfo , &time) ;
#endif

#if defined(LINUX)
    ::localtime_r(&time , &timeinfo) ;
#endif

    char ctime[50];
    sprintf(ctime,"%04d-%02d-%02d %02d:%02d:%02d",
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec
        );
    return string(ctime);
}

string
SSTable::row_type::bit64_to_string(uint64 bit64) const
{
    std::bitset<32> l32(static_cast<unsigned long>(bit64));
    std::bitset<32> h32(bit64 >> 32);
    string str_bit = h32.to_string<char,std::char_traits<char>,std::allocator<char> >() + l32.to_string<char,std::char_traits<char>,std::allocator<char> >();
    return str_bit;
}

time_t
SSTable::row_type::string_to_t32(const string &time) const
{
    tm timeinfo;
    memset(&timeinfo,0, sizeof(timeinfo));
    if(time.size() > 10){
        sscanf(time.c_str(),"%04d-%02d-%02d %02d:%02d:%02d",&timeinfo.tm_year,&timeinfo.tm_mon,&timeinfo.tm_mday,&timeinfo.tm_hour,&timeinfo.tm_min,&timeinfo.tm_sec);
    }
    else{
        sscanf(time.c_str(),"%04d-%02d-%02d",&timeinfo.tm_year,&timeinfo.tm_mon,&timeinfo.tm_mday);
    }

    timeinfo.tm_year -= 1900;
    timeinfo.tm_mon--;

    time_t clock;
    clock = mktime(&timeinfo);
    return clock;
}

uint64
SSTable::row_type::string_to_bit64(const string &bit64) const
{
    string l32;
    string h32;
    if(bit64.size() > 64){
        h32 = bit64.substr(bit64.size() - 64, 32);
        l32 = bit64.substr(bit64.size() - 32, 32);
    }
    else if(bit64.size() > 32){
        h32 = bit64.substr(0, bit64.size() - 32);
        l32 = bit64.substr(bit64.size() - 32, 32);
    }
    else{
        l32 = bit64;
    }

    std::bitset<32> bl32(l32);
    std::bitset<32> bh32(h32);

    uint64 ret;
    ret = (static_cast<uint64>(bh32.to_ulong()) << 32) + bl32.to_ulong();

    return ret;
}


bool
SSTable::load(const string &param, const string& sqlstr)
{
    clear();
    if(param == FILE){
        if(false == load_from_file(sqlstr.c_str())){
            return false;
        }
    }
    else{
	#if defined (MYSQL_DRIVER)
        m_type = "mysql";
	#endif
	#if defined (ODBC_DRIVER)
        if("odbc" == param.substr(0,4)){
            m_type = "odbc";
        }
	#endif

	#if defined(OCI_DRIVER)
        if ("oci:" == param.substr(0,4))
        {
             m_type = "oci";
        }
    #endif
	if (m_type != "mysql" && m_type != "odbc" && m_type != "oci")
		{
			DA_ERRLOG("Parameter format error: %s" , param.c_str());
			DA_FLUSH(ERROR_FLUSH);
			return false;
		}
        scoped_ptr<ss::db_driver> driverPtr(ss::db_driver::create(m_type));
        if(NULL == driverPtr){
            DA_ERRLOG("%s: ss::db_driver::create() failed [%s,%s]",__FUNCTION__, param.c_str(), sqlstr.c_str());
            DA_FLUSH(ERROR_FLUSH);
            return false;
        }
        
        if(false == driverPtr->open(param)){
            m_errmsg = driverPtr->errmsg();
            m_errcode = driverPtr->errcode();
            DA_ERRLOG("%s: driverPtr->open(%s) failed [%s]",__FUNCTION__, param.c_str(), m_errmsg.c_str());
            DA_FLUSH(ERROR_FLUSH);
            return false;
        }
        if(false == driverPtr->query(m_result, sqlstr.c_str())){
            m_errmsg = driverPtr->errmsg();
            m_errcode = driverPtr->errcode();
            DA_ERRLOG("%s: driverPtr->query(%s) failed [%s]",__FUNCTION__, sqlstr.c_str(), m_errmsg.c_str());
            DA_FLUSH(ERROR_FLUSH);
            return false;
        }
    }

    m_param = param;
    m_sqlstr = sqlstr;
    m_init = true;
    return true;
}

bool
SSTable::resize(int new_size)
{
    if(new_size <= size()){
        return true;
    }
    int increase_size = new_size - size();
    row_type new_row = get_empty_row();
    while(increase_size){
        if(false == push_back(new_row)){
            return false;
        }
        increase_size--;
    }

    return true;
}

SSTable::const_iterator
SSTable::begin() const
{
    if(0 == m_result.row_count()){
        return ROW_NULL;
    }
    return SSTable::row_type(this,*m_result.next(NULL));
}
SSTable::const_iterator
SSTable::end() const
{
    return ROW_NULL;
}

SSTable::const_iterator
SSTable::last() const
{
    if(0 == m_result.row_count()){
        return ROW_NULL;
    }
    return SSTable::row_type(this,*m_result.prev(NULL));
}

SSTable::iterator
SSTable::begin()
{
    if(0 == m_result.row_count()){
        return ROW_NULL;
    }
    return SSTable::row_type(this,*m_result.next(NULL));
}

SSTable::iterator
SSTable::end()
{
    return ROW_NULL;
}

SSTable::iterator
SSTable::last()
{
    if(0 == m_result.row_count()){
        return ROW_NULL;
    }
    return SSTable::row_type(this,*m_result.prev(NULL));
}

bool
SSTable::commit()
{
    return commit_ex(NULL);
}

bool
SSTable::commit_ex(vector<string> *sql_statements)
{
    if(m_param == FILE){
        return save_to_file(m_sqlstr.c_str());
    }
    scoped_ptr<ss::db_driver> driverPtr(ss::db_driver::create(m_type));
    if(NULL == driverPtr){
        DA_ERRLOG("%s: ss::db_driver::create() failed [%s,%s]",__FUNCTION__, m_param.c_str(), m_sqlstr.c_str());
        DA_FLUSH(ERROR_FLUSH);
        return false;
    }

    if(false == driverPtr->open(m_param)){
        m_errmsg = driverPtr->errmsg();
        m_errcode = driverPtr->errcode();
        DA_ERRLOG("%s: driverPtr->open(%s) failed [%s]",__FUNCTION__, m_param.c_str(), m_errmsg.c_str());
        DA_FLUSH(ERROR_FLUSH);
        return false;
    }

    boost::shared_ptr<SSSQLCreator> spSQLCreator = SSSQLCreator::CreateByType(m_type);

    driverPtr->begin();
    string tablename = m_result.get_header().fields(0).table_name;
    spSQLCreator->SetTableName(tablename);
    spSQLCreator->SetDBDriver(driverPtr.get());
    string sqlstr;
    bool succ = true;
    //delete action is the highest priority
    std::list<std::string> lstRet_Delete;
    if (spSQLCreator->ParseDelete(m_delete_list, this, lstRet_Delete))
    {
        for (std::list<std::string>::iterator iter = lstRet_Delete.begin(); iter != lstRet_Delete.end(); ++iter)
        {
            if (!driverPtr->execute(iter->c_str()))
            {
                m_errmsg = driverPtr->errmsg();
                m_errcode = driverPtr->errcode();
                DA_ERRLOG("%s:execute error [%s][%s]",__FUNCTION__, sqlstr.c_str(), m_errmsg.c_str());
                succ = false;
                break;
            }

            DA_FILELOG(*iter);
            if(NULL != sql_statements)
            {
                sql_statements->push_back(*iter);
            }
        }
    }
    /*
    for(set<ss::block *>::iterator it = m_delete_list.begin(); it != m_delete_list.end(); ++it){
        ss::db_row row(*it);
        sqlstr = sql_delete(driverPtr.get(), tablename, row);
        if(false == driverPtr->execute(sqlstr.c_str())){
            m_errmsg = driverPtr->errmsg();
            m_errcode = driverPtr->errcode();
            DA_ERRLOG("%s:execute error [%s][%s]",__FUNCTION__, sqlstr.c_str(), m_errmsg.c_str());
            DA_FLUSH(ERROR_FLUSH);
            succ = false;
            break;
        }
        DA_FILELOG(sqlstr);
        if(NULL != sql_statements){
            sql_statements->push_back(sqlstr);
        }
    }
    */

    if(true == succ){
        //then insert, final modified
        std::list<std::string> lstRet_MI;
        if (spSQLCreator->ParseInsertAndModify(m_insert_and_modify_list_timeorder, this, lstRet_MI))
        {
            for (std::list<std::string>::iterator iter = lstRet_MI.begin(); iter != lstRet_MI.end(); ++iter)
            {
                if (!driverPtr->execute(iter->c_str()))
                {
                    m_errmsg = driverPtr->errmsg();
                    m_errcode = driverPtr->errcode();
                    DA_ERRLOG("%s:execute error [%s][%s]",__FUNCTION__, sqlstr.c_str(), m_errmsg.c_str());
                    succ = false;
                    break;
                }

                DA_FILELOG(*iter);
                if(NULL != sql_statements)
                {
                    sql_statements->push_back(*iter);
                }
            }
        }
        /*
        for(set<ss::block *>::iterator it = m_insert_and_modify_list.begin(); it != m_insert_and_modify_list.end(); ++it){
            ss::db_row row(*it);
            if(row_type(this,row).is_inserted()){
                sqlstr = sql_insert(driverPtr.get(), tablename, row);
            }
            else if(row_type(this,row).is_modified()){
                sqlstr = sql_modified(driverPtr.get(), tablename, row);
            }
            else{
                continue;
            }
            if(false == driverPtr->execute(sqlstr.c_str())){
                m_errmsg = driverPtr->errmsg();
                m_errcode = driverPtr->errcode();
                DA_ERRLOG("%s:execute error [%s][%s]",__FUNCTION__, sqlstr.c_str(), m_errmsg.c_str());
                succ = false;
                break;
            }
            DA_FILELOG(sqlstr);
            if(NULL != sql_statements){
                sql_statements->push_back(sqlstr);
            }
        }
        */
    }

    if(false == succ){
        driverPtr->rollback();
    }
    else{
        succ = driverPtr->commit();
    }

    m_errmsg = driverPtr->errmsg();
    m_errcode = driverPtr->errcode();

    if(true == succ){
        if(m_delete_list.size()){
            const ss::db_row *row = NULL;
            while((row = const_cast<ss::db_row *>(m_result.next(row))) != NULL){
                clear_row_insert_flag(row);
                clear_row_modified_flag(row);
                row = clear_row_delete_flag(const_cast<ss::db_row *>(row));
            }
        }
        else{
            for(map<int, ss::block *>::iterator it = m_insert_and_modify_list_timeorder.begin(); it != m_insert_and_modify_list_timeorder.end(); ++it){
                ss::db_row r(it->second);
                clear_row_insert_flag(&r);
                clear_row_modified_flag(&r);
            }
        }
        clear_transaction();
    }
    else{
        DA_ERRLOG("%s:failed [%s,%s][%s]",__FUNCTION__, m_param.c_str(), m_sqlstr.c_str(), m_errmsg.c_str());
    }
    DA_FLUSH(DATA_FLUSH);
    DA_FLUSH(ERROR_FLUSH);
    return succ;
}


void
SSTable::clear_row_modified_flag(const ss::db_row *row)
{
    char *buf = row->buffer();
    if(NULL == buf || 0 == row->length()){
        return;
    }
    ss::field *field = reinterpret_cast<ss::field *>(buf);
    for(int i = 0; i < m_result.get_header().field_count(); ++i){
        ss::set_bit_mask(field[i].holder()->flag, 3, false);
    }
    return;
}
void
SSTable::clear_row_insert_flag(const ss::db_row *row)
{
    char *buf = row->buffer();
    if(NULL == buf || 0 == row->length()){
        return;
    }
    ss::field *field = reinterpret_cast<ss::field *>(buf);
    ss::set_bit_mask(field[0].holder()->flag, 4, false);
    return ;
}

const ss::db_row *
SSTable::clear_row_delete_flag(ss::db_row *row)
{
    const ss::db_row *prev_row = NULL;
    if(row_type(this,*row).is_deleted()){
        prev_row = m_result.prev(row);
        m_result.del_row(row);
        return prev_row;
    }
    return row;
}

SSTable::row_type
SSTable::get_empty_row() const
{
    int size = sizeof(ss::field) * m_result.get_header().field_count();
    ss::block * block = new ss::block(size);
    if(NULL == block){
        DA_ERRLOG("%s: NULL == new ss::block(%d)",__FUNCTION__, size);
        DA_FLUSH(ERROR_FLUSH);
        return row_type();
    }
    char * buffer = (char *)block->buffer() ;
    if(NULL == buffer){
        DA_ERRLOG("%s: warning! NULL == (char *)block->buffer()[%d]",__FUNCTION__, size);
    }
    else{
        memset(buffer , 0 , size) ;
        ss::__field_holder *fields = (ss::__field_holder *)buffer ;

        for(int idx = 0 ; idx < m_result.get_header().field_count() ; idx++)
        {
            fields[idx].type = m_result.get_header().fields(idx).type;
            fields[idx].magic = 0x5555 ;
        }//endif  for ....
        block->length(size);
    }

    row_type row(this, block);
    row.m_new_line = true;
    DA_FLUSH(ERROR_FLUSH);
    return row;
}

bool
SSTable::push_back(const row_type &row)
{
    char *src = row.m_row.buffer();
    if(NULL == src){
        DA_ERRLOG("%s: NULL == src",__FUNCTION__);
        DA_FLUSH(ERROR_FLUSH);
        return false;
    }
    ss::block * block = new ss::block(row.m_row.length());
    if(NULL == block){
        DA_ERRLOG("%s: NULL == new ss::block(%d)",__FUNCTION__, row.m_row.length());
        DA_FLUSH(ERROR_FLUSH);
        return false;
    }
    char *dst = block->buffer();
    if(NULL == dst){
        DA_ERRLOG("%s: NULL == dst",__FUNCTION__);
        DA_FLUSH(ERROR_FLUSH);
        delete block;
        return false;
    }
    memcpy(dst, src, row.m_row.length());

    block->length(row.m_row.length());
    ss::db_row * db_row = new ss::db_row(block);
    if(NULL == db_row){
        DA_ERRLOG("%s: NULL == dst",__FUNCTION__);
        DA_FLUSH(ERROR_FLUSH);
        delete block;
        return false;
    }

    ss::__field_holder *fields = (ss::__field_holder *)dst ;
    for(int idx = 0 ; idx < m_result.get_header().field_count() ; idx++){
        if(fields[idx].type == ss::SSTYPE_STRING && NULL != fields[idx].data.str){
            int length = ::strlen(fields[idx].data.str) + 1;
            char * pchar = (char *)malloc(length) ;
            if(pchar != NULL){
                memcpy(pchar , fields[idx].data.str , length) ;
                fields[idx].data.str =  pchar;
            }
            else{
                DA_ERRLOG("%s: NULL == (char *)malloc(%d)",__FUNCTION__, length);
            }
        }
        else if(fields[idx].type == ss::SSTYPE_RAW && NULL != fields[idx].data.raw){
            uint32 length = fields[idx].length;
            char * praw = (char *)malloc(length) ;
            if(praw != NULL){
                memcpy(praw , fields[idx].data.raw, length) ;
                fields[idx].data.raw =  praw;
            }
            else{
                DA_ERRLOG("%s: NULL == (char *)malloc(%d)",__FUNCTION__, length);
            }
        }
    }//endif  for ....
    //set insert flag
    ss::set_bit_mask(fields[0].flag,4,true);
    add_insert_and_modify_block(db_row->get_block());
    m_result.add_row(db_row);
    DA_FLUSH(ERROR_FLUSH);
    return true;
}

bool
SSTable::push_back_reference(const row_type &row)
{
    ss::db_row * db_row = new ss::db_row;
    if(NULL == db_row){
        DA_ERRLOG("%s: NULL == dst",__FUNCTION__);
        DA_FLUSH(ERROR_FLUSH);
        return false;
    }

    (*db_row) = row.m_row;
    m_result.add_row(db_row);
    return true;
}

// erase one row
void
SSTable::erase(const_iterator &iter)
{
    //the code below is strange, the purpose is to point the correct memory of the node(db_row) in the original list
    m_result.del_row((ss::db_row *)((iter.m_items.m_row.prev)->next));

    //remove from commit list
    erase_insert_and_modify_block(iter->m_row.get_block());
    m_delete_list.erase(iter->m_row.get_block());
}

void
SSTable::erase(iterator &iter)
{
    //the code below is strange, the purpose is to point the correct memory of the node(db_row) in the original list
    m_result.del_row((ss::db_row *)((iter.m_items.m_row.prev)->next));

    //remove from commit list
    erase_insert_and_modify_block(iter->m_row.get_block());
    m_delete_list.erase(iter->m_row.get_block());
}

/*
string
SSTable::sql_modified(const ss::db_driver *driver, const string &tablename, const ss::db_row& row) const
{
    row_type value(this, row);
    string sqlstr = "UPDATE " + tablename + " SET ";
    for(int i = 0; i < m_result.get_header().field_count(); ++i){
        string name = m_result.get_header().fields(i).name;
        if(false == value.is_modified(name)){
            continue;
        }

        sqlstr += driver->format_col_name(name);
        sqlstr += " = ";
        if(!value.assigned(name)){
            sqlstr += "NULL";
        }
    
        string sfmtval = driver->format_val(m_result.get_header().fields(i).type, value.get<string>(name));
        sqlstr += sfmtval;

        sqlstr += ",";
    }
    //remove the last ,
    sqlstr.erase(sqlstr.end() - 1);

    //where statement
    string where = " WHERE";
    for(int i = 0; i < m_result.get_header().primary_count(); ++i){
        if(i){
            where += " AND ";
        }
        string name = m_result.get_header().get_primary(i);
        //where += (" " + driver->format_col_name(name) + " = \'" + value.get<string>(name) + "\'");
        where += (" " + driver->format_col_name(name) + " = " + driver->format_val(m_result.get_header().fields(name).type, value.get<string>(name)));
    }
    sqlstr += where;
    return sqlstr;
}

string
SSTable::sql_insert(const ss::db_driver *driver, const string &tablename, const ss::db_row& row) const
{
    string sqlstr;
    if(1 == m_insert_mode){
        sqlstr = "REPLACE INTO " + tablename + "(";
    }
    else{
        sqlstr = "INSERT INTO " + tablename + "(";
    }
    string values = "VALUES (";
    string on_duplcate_key_update;
    row_type value(this, row);
    for(int i = 0; i < m_result.get_header().field_count(); ++i){
        string name = m_result.get_header().fields(i).name;
        if(!value[name].assigned()){
            continue;
        }

        sqlstr += driver->format_col_name(name);
        sqlstr += ",";

        string sfmtval = driver->format_val(value[name].type(), value.get<string>(name));
        values += sfmtval;

        if(value.get<string>(name).empty())
        {
            DA_ERRLOG("%s: value.get<string>(%s).empty():[%s,%s]", __FUNCTION__, name.c_str(), sqlstr.c_str(), values.c_str());
        }

        values += ",";

        if(2 == m_insert_mode){
            on_duplcate_key_update += driver->format_col_name(name);
            on_duplcate_key_update += " = ";
            if(!value.assigned(name)){
                on_duplcate_key_update += "NULL";
            }

            string sfmtval = driver->format_val(m_result.get_header().fields(i).type, value.get<string>(name));
            on_duplcate_key_update += sfmtval;


            on_duplcate_key_update += ",";
        }
    }
    //replace the last , as )
    sqlstr[sqlstr.size() - 1] = ')';
    values[values.size() - 1] = ')';
    sqlstr += " ";
    sqlstr += values;
    if(2 == m_insert_mode){
        on_duplcate_key_update[on_duplcate_key_update.size() - 1] = ' ';
        sqlstr += " ON DUPLICATE KEY UPDATE ";
        sqlstr += on_duplcate_key_update;
    }
    return sqlstr;
}

string
SSTable::sql_delete(const ss::db_driver *driver, const string &tablename, const ss::db_row& row) const
{
    string sqlstr = "DELETE FROM " + tablename + " WHERE ";
    row_type value(this, row);
    for(int i = 0; i < m_result.get_header().primary_count(); ++i){
        if(i){
            sqlstr += " AND ";
        }
        string name = m_result.get_header().get_primary(i);
        //sqlstr += (driver->format_col_name(name) + " = \'" + value.get<string>(name) + "\'");
        sqlstr += (driver->format_col_name(name) + " = " + driver->format_val(m_result.get_header().fields(name).type, value.get<string>(name)));
    }
    return sqlstr;
}
*/

void
SSTable::clear()
{
    m_param.clear();
    m_sqlstr.clear();
    m_type.clear();
    m_result.reset();
    m_init = false;
    m_field_name_to_lower = false;
    m_insert_mode = 0;
    clear_data();
}

//column count
int
SSTable::get_field_count() const
{
    return m_result.get_header().field_count();
}

//column name
const string &
SSTable::get_field_name(int cnt) const
{
    if(m_field_name_to_lower){
        return m_result.get_lower_case_header().fields(cnt).name;
    }
    else{
        return m_result.get_header().fields(cnt).name;
    }
}

// check if the field name is existed of the table
bool
SSTable::count(const string &name) const
{
    int cnt;
    if(m_field_name_to_lower){
        cnt = m_result.get_lower_case_header().find(to_lower_copy(name));
    }
    else{
        cnt = m_result.get_header().find(name);
    }
    return -1 != cnt;

}

// clear all the content, remain table head
void
SSTable::clear_data()
{
    m_result.clear();
    m_errmsg.clear();
    m_errcode = 0;
    if(NULL != m_row_mapping_array_cache){
        delete m_row_mapping_array_cache;
        m_row_mapping_array_cache = NULL;
    }
    clear_transaction();
}

bool
SSTable::const_iterator::operator==(const SSTable::const_iterator& iter) const
{
    return m_items == iter.m_items;
}

bool
SSTable::const_iterator::operator!=(const SSTable::const_iterator& iter) const
{
    return !(*this == iter);
}

bool
SSTable::const_iterator::operator==(const SSTable::iterator& iter) const
{
    return m_items == iter.m_items;
}

bool
SSTable::const_iterator::operator!=(const SSTable::iterator& iter) const
{
    return !(*this == iter);
}


SSTable::const_iterator& SSTable::const_iterator::operator++()
{
    if(false == m_items.next()){
        m_items = ROW_NULL;
    }
    return (*this);
}

SSTable::const_iterator SSTable::const_iterator::operator++(int)
{
    SSTable::const_iterator tmp = *this;
    if(false == m_items.next()){
        m_items = ROW_NULL;
    }
    return tmp;
}

SSTable::const_iterator& SSTable::const_iterator::operator--()
{
    if(false == m_items.prev()){
        m_items = ROW_NULL;
    }
    return (*this);
}

SSTable::const_iterator SSTable::const_iterator::operator--(int)
{
    SSTable::const_iterator tmp = *this;
    if(false == m_items.prev()){
        m_items = ROW_NULL;
    }
    return tmp;
}

SSTable::iterator& SSTable::iterator::operator++()
{
    if(false == m_items.next()){
        m_items = ROW_NULL;
    }
    return (*this);
}

SSTable::iterator SSTable::iterator::operator++(int)
{
    SSTable::iterator tmp = *this;
    if(false == m_items.next()){
        m_items = ROW_NULL;
    }
    return tmp;
}

SSTable::iterator& SSTable::iterator::operator--()
{
    if(false == m_items.prev()){
        m_items = ROW_NULL;
    }
    return (*this);
}

SSTable::iterator SSTable::iterator::operator--(int)
{
    SSTable::iterator tmp = *this;
    if(false == m_items.prev()){
        m_items = ROW_NULL;
    }
    return tmp;
}

bool
SSTable::save_to_file(const char *filename) const
{
    vector<ss::block_ref> blocks;
    int tlen = 0;
    tlen = m_result.dump(blocks);
    if(0 == tlen){
        m_errmsg = "total save buffer size is 0";
        return false;
    }

    std::ofstream of(filename, std::ios_base::binary);
    if(!of.is_open()){
        m_errmsg = "open file failed";
        return false;
    }

    of << VER;
    for(vector<ss::block_ref>::const_iterator iter = blocks.begin(); iter != blocks.end(); ++iter){
        ss::block *block = iter->get_block();
        int size = block->length();
        char *buf = block->buffer();
        of.write(reinterpret_cast<char *>(&size), sizeof(size));;
        of.write(buf, size);
    }
    of.close();
    return true;
}

bool
SSTable::load_from_file(const char *filename)
{
    std::ifstream inf(filename, std::ios_base::binary);
    if(!inf.is_open()){
        m_errmsg = "open file failed";
        return false;
    }

    char ver[10];
    memset(ver, 0, sizeof(ver));
    inf.read(ver,6);
    if(ver != VER){
        m_errmsg = "version is not same";
        return false;
    }

    vector<ss::block_ref> blocks;
    int size = 0;
    while(!inf.eof()){
        inf.read(reinterpret_cast<char *>(&size), sizeof(size));
        if(0 == inf.gcount()){
            break;
        }
        ss::block * block = new ss::block(size);
        char *buffer = block->buffer();
        inf.read(buffer, size);
        block->length(size);
        ss::block_ref ref(block , true);
        blocks.push_back(ref) ;
    }
    m_result.restore(blocks);
    return true;
}

void
SSTable::add_insert_and_modify_block(ss::block * blk)
{
    map<ss::block *, int>::iterator i = m_insert_and_modify_list.find(blk);
    if (i != m_insert_and_modify_list.end()) {
        if (i->second == m_insert_and_modify_id - 1) {
            return;
        } else {
            m_insert_and_modify_list_timeorder.erase(i->second);
            i->second = m_insert_and_modify_id++;
            m_insert_and_modify_list_timeorder.insert(make_pair(i->second, blk));
        }
    } else {
        m_insert_and_modify_list.insert(make_pair(blk, m_insert_and_modify_id));
        m_insert_and_modify_list_timeorder.insert(make_pair(m_insert_and_modify_id, blk));
        m_insert_and_modify_id++;
    }
}

void
SSTable::erase_insert_and_modify_block(ss::block * blk)
{
    map<ss::block *, int>::iterator i = m_insert_and_modify_list.find(blk);
    if (i != m_insert_and_modify_list.end()) {
        m_insert_and_modify_list_timeorder.erase(i->second);
        m_insert_and_modify_list.erase(i);
    }
}

void
SSTable::row_mapping_array_cache() const
{
    if(NULL != m_row_mapping_array_cache){
        delete m_row_mapping_array_cache;
        m_row_mapping_array_cache = NULL;
    }
    m_row_mapping_array_cache = new vector<row_type>;
    if(NULL == m_row_mapping_array_cache){
        DA_ERRLOG("%s: new vector<iterator> failed",__FUNCTION__);
        DA_FLUSH(ERROR_FLUSH);
        return;
    }
    for(const_iterator it = begin(); it != end(); ++it){
        m_row_mapping_array_cache->push_back(it);
    }
    return;
}

#if 0
SSTable::row_type &
SSTable::operator[](int n)
{
    if(NULL == m_row_mapping_array_cache){
        row_mapping_array_cache();
    }
    return (*m_row_mapping_array_cache)[n];
}
#endif

const SSTable::row_type &
SSTable::operator[](int n) const
{
    if(NULL == m_row_mapping_array_cache){
        row_mapping_array_cache();
    }
    return (*m_row_mapping_array_cache)[n];
}

SSTable &
SSTable::operator=(const SSTable &rhs)
{
    if(this == &rhs){
        return (*this);
    }
    clear();
    m_init = rhs.m_init;
    m_param = rhs.m_param;
    m_sqlstr = rhs.m_sqlstr;
    m_errmsg = rhs.m_errmsg;
    m_errcode = rhs.m_errcode;
    m_result.set_header(rhs.m_result.get_header());
    for(const_iterator iter = rhs.begin(); iter != rhs.end(); ++iter){
        push_back(*iter);
    }
    return (*this);
}

//column count
int
SSTable::row_type::get_field_count() const
{
    return m_table->get_field_count();
}

//column name
const string &
SSTable::row_type::get_field_name(int cnt) const
{
    return m_table->get_field_name(cnt);
}



bool
SSTable::row_type::equal_to(const string &name, const row_type &rhs) const
{
    return compare_to_impl(name, rhs, ss_equal_to());
}

bool
SSTable::row_type::not_equal_to(const string &name, const row_type &rhs) const
{
    return compare_to_impl(name, rhs, ss_not_equal_to());
}


bool
SSTable::row_type::less(const string &name, const row_type &rhs) const
{
    return compare_to_impl(name, rhs, ss_less());
}
bool
SSTable::row_type::less_equal(const string &name, const row_type &rhs) const
{
    return compare_to_impl(name, rhs, ss_less_equal());
}
bool
SSTable::row_type::greater(const string &name, const row_type &rhs) const
{
    return compare_to_impl(name, rhs, ss_greater());
}
bool
SSTable::row_type::greater_equal(const string &name, const row_type &rhs) const
{
    return compare_to_impl(name, rhs, ss_greater_equal());
}

bool
SSTable::row_type::is_modified() const
{
    char *buf = m_row.buffer();
    if(NULL == buf || 0 == m_row.length()){
        return false;
    }
    ss::field *field = reinterpret_cast<ss::field *>(buf);
    for(int i = 0; i < get_field_count(); ++i){
        if(ss::get_bit_mask(field[i].holder()->flag, 3)){
            return true;
        }
    }
    return false;
}

bool
SSTable::row_type::is_inserted() const
{
    char *buf = m_row.buffer();
    if(NULL == buf || 0 == m_row.length()){
        return false;
    }
    ss::field *field = reinterpret_cast<ss::field *>(buf);
    return ss::get_bit_mask(field[0].holder()->flag,4);
}

bool
SSTable::row_type::is_deleted() const
{
    char *buf = m_row.buffer();
    if(NULL == buf || 0 == m_row.length()){
        return false;
    }
    ss::field *field = reinterpret_cast<ss::field *>(buf);
    return ss::get_bit_mask(field[0].holder()->flag,5);
}

void
SSTable::clear_transaction()
{
    m_insert_and_modify_id = 0;
    m_insert_and_modify_list.clear();
    m_insert_and_modify_list_timeorder.clear();
    m_delete_list.clear();
}

int
SSTable::primary_count() const
{
    return m_result.get_header().primary_count();
}

const string &
SSTable::get_primary(int i) const
{
    if(m_field_name_to_lower){
        return m_result.get_lower_case_header().get_primary(i);
    }
    else{
        return m_result.get_header().get_primary(i);
    }
}

//execute the sql statement direct
int
SSTable::execute(const string &param, const vector<string> &sql, string &error_msg)
{
    int error_code = 0;
    string sdbtype = "mysql";
	//string sdbtype = "oci";
    if("odbc" == param.substr(0,4)){
        sdbtype = "odbc";
    }
    else if ("oci:" == param.substr(0,4))
    {
        sdbtype = "oci";
    }
    scoped_ptr<ss::db_driver> driverPtr(ss::db_driver::create(sdbtype));
    if(NULL == driverPtr){
        DA_ERRLOG("%s: ss::db_driver::create() failed [%s]",__FUNCTION__, param.c_str());
        DA_FLUSH(ERROR_FLUSH);
        return -1;
    }

    if(false == driverPtr->open(param)){
        error_msg = driverPtr->errmsg();
        error_code = driverPtr->errcode();
        DA_ERRLOG("%s: driverPtr->open(%s) failed [%s]",__FUNCTION__, param.c_str(), error_msg.c_str());
        DA_FLUSH(ERROR_FLUSH);
        return error_code;
    }

    driverPtr->begin();
    for(vector<string>::const_iterator it = sql.begin(); it != sql.end(); ++it){
        if(false == driverPtr->execute(it->c_str())){
            error_msg = driverPtr->errmsg();
            error_code = driverPtr->errcode();
            DA_ERRLOG("%s:execute error [%s][%s]",__FUNCTION__, it->c_str(), error_msg.c_str());
            DA_FLUSH(ERROR_FLUSH);
            driverPtr->rollback();
            return error_code;
        }
        DA_FILELOG(*it);
    }


    bool succ = driverPtr->commit();
    if(false == succ){
        error_msg = driverPtr->errmsg();
        error_code = driverPtr->errcode();
    }
    DA_FLUSH(DATA_FLUSH);
    return error_code;
}

 bool SSTable::dumpForPatch(std::string& strBuf) const
 {
     std::vector<ss::block_ref> blocks;

     int tlen = m_result.dumpForPatch(blocks);

     int ilen = tlen;
     int buflen = 0 ;
     char *bufi = (char *)malloc(tlen) ;
     for (int idx = 0; idx<(int)blocks.size(); idx++)
     {
         char * buffer = blocks[idx].buffer() ;
         int length = blocks[idx].length() ;

         if(buflen + length > ilen)
         {
             ::free(bufi) ;
             return false;
         }

         memcpy(bufi + buflen , buffer , length) ;
         buflen += length ;	
     }

     blocks.clear();
     strBuf.assign(bufi, ilen);

     ::free(bufi);

     return true;


 }

} //namespace
