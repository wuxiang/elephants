#if defined(ODBC_DRIVER)

#include <string>
using namespace std;

#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/ODBC, MS SQL 2008
// #define OTL_STL // Turn on STL features
//#define OTL_STREAM_READ_ITERATOR_ON
#ifdef WIN32
#define OTL_BIGINT __int64
#else
#define OTL_BIGINT int64_t
#define OTL_ODBC_UNIX
#endif
 #if defined(__GNUC__)
 
namespace std{
 	typedef unsigned short unicode_char;
 	typedef basic_string<unicode_char> unicode_string;
}
 
#define OTL_UNICODE_CHAR_TYPE unicode_char
#define OTL_UNICODE_STRING_TYPE unicode_string
 
#else

#define OTL_UNICODE_CHAR_TYPE wchar_t
#define OTL_UNICODE_STRING_TYPE wstring
#endif

#define OTL_UNICODE
#include "Driver/otlv4.h"
#include "Driver/ODBCDriver.h"
#include "SSLog.h"
#include <sstream>
#include <boost/locale.hpp>

boost::mutex odbc_driver::m_lock;
bool odbc_driver::init_ = false;


odbc_driver::odbc_driver(const char * str) 
    : db_driver("odbc")
{
    m_lock.lock();
    if(!init_){
        otl_connect::otl_initialize(); // initialize ODBC environment
        init_ = true;
    }
    conn_ = NULL ;
}

odbc_driver::~odbc_driver()
{
    LOGCRIT("we will close mysql connection[%s:%s:%s:%s]" , host_.c_str() , port_.c_str() , username_.c_str() , database_.c_str()) ;
    close() ;
    m_lock.unlock();
}

bool odbc_driver::open(const std::string& param)
{
    split_param(param) ;
    conn_ = new otl_connect;
    if(NULL == conn_){
        return false;
    }

    string connect_str;
    connect_str += username_;
    connect_str += "/";
    connect_str += password_;
    connect_str += "@";
    connect_str += database_;

    try{
        conn_->rlogon(connect_str.c_str());
    }
    catch(otl_exception& p){ // intercept OTL exceptions
        errcode_ = p.code;
        ostringstream o;
        o << p.sqlstate << "|" << p.msg << "|" << p.stm_text << "|" << p.var_info;
        errmsg_ = o.str();
        return false;
    }
    return true;
}

void odbc_driver::close()
{
    if(conn_ != NULL)
    {
        conn_->logoff() ;
        delete conn_;
        conn_ = NULL ;	
    }
}

bool odbc_driver::execute(const char * sqlstr)
{
    return true;
}


bool odbc_driver::query(ss::db_result& result , const char * sqlstr , int limmited)
{
    result.clear() ;
    if(sqlstr == NULL)
        return false ;

    try{
        otl_stream i(1024, // buffer size
                    sqlstr, // SELECT statement
                    *conn_ // connect object
                    ); 

        ss::db_result::header header ;
        get_header(header , i) ;
        result.set_header(header) ;
        ss::uint32 fc = header.field_count();


        while(!i.eof()){
            int size = sizeof(ss::field) * fc ;
            int tlen = size ;
            ss::block * block = new ss::block(tlen) ;
            if(NULL == block){
                return false;
            }
            char * buffer = (char *)block->buffer() ;
            if(NULL == buffer){
                return false;
            }
            memset(buffer , 0 , tlen) ;

            ss::__field_holder *fields = (ss::__field_holder *)buffer ;
            for(ss::uint32 idx = 0; idx < fc ; idx++)
            {
                int type = header.fields(idx).type ;
                fields[idx].type = type ;
                fields[idx].magic = 0x5555 ;
                fields[idx].length = ss::get_type_size(type) ;
                fields[idx].data.val = 0 ;

                bool assigned = true ;
                switch(type)
                {
                case ss::SSTYPE_INT8 : 
                    assigned = false ;
//                     i >> fields[idx].data.i8;
                    break ;
                case ss::SSTYPE_INT16 : 
                    i >> fields[idx].data.i16;
                    break ;
                case ss::SSTYPE_INT32 : 
                    i >> fields[idx].data.i32;
                    break ;
                case ss::SSTYPE_INT64 : 
                    i >> fields[idx].data.i64;
                    break ;
                case ss::SSTYPE_FLOAT : 
                    i >> fields[idx].data.f32;
                    break ;
                case ss::SSTYPE_DOUBLE : 
                    i >> fields[idx].data.f64;
                    break ;
                case ss::SSTYPE_STRING :
                    {
                        OTL_UNICODE_STRING_TYPE tmp_str;
                        i >> tmp_str;
                        string utf8;
                        utf8 = boost::locale::conv::utf_to_utf<char>(tmp_str);
                        int strsize = utf8.size() + 1;
                        char * pchar = (char *)malloc(strsize);
                        if(NULL == pchar){
                            assigned = false ;
                        }
                        else{
                            memcpy(pchar , utf8.c_str() , strsize) ;
                            fields[idx].data.str = pchar;
                            fields[idx].length = strsize;
                        }
                        break ;
                    }
                case ss::SSTYPE_T32 :
                    assigned = false ;
                    break ;
                case ss::SSTYPE_T64 :
                    {
                        otl_datetime datetime;
                        i >> datetime;
                        time_t time = ss::make_time(datetime.year, datetime.month, datetime.day, datetime.hour, datetime.minute, datetime.second);
                        if(-1 == time){
                            assigned = false;
                        }
                        else{
                            fields[idx].data.t64 = (ss::int32)time ;
                            fields[idx].data.t64 = (fields[idx].data.t64 * 1000) ;
                        }
                        break ;
                    }
                case ss::SSTYPE_BIT64 :
                    {
                        assigned = false ;
                        break ;
                    }
                default:
                    assigned = false ;
                    break ;
                }		

                if(assigned == true)
                {
                    assigned = (i.is_null() == 0);
                    ss::set_bit_mask(fields[idx].flag , 0 , assigned) ;
                }
            }//endif  for ....

            block->length(tlen) ;

            ss::db_row * db_row = new ss::db_row(block) ;
            if(NULL == db_row){
                return false;
            }
            db_row->attach(block) ;

            result.add_row(db_row) ;
        }//end if while ...

    }
    catch(otl_exception& p){ // intercept OTL exceptions
        errcode_ = p.code;
        ostringstream o;
        o << p.sqlstate << "|" << p.msg << "|" << p.stm_text << "|" << p.var_info;
        errmsg_ = o.str();
        return false;
    }
    return true ;
}

bool
odbc_driver::get_header(ss::db_result::header& header , otl_stream &i) const
{
    const otl_column_desc* desc;
    int desc_len;

    desc=i.describe_select(desc_len);

    for(int n=0;n<desc_len;++n){
        header.add(desc[n].name ,"", native_type(desc[n].otl_var_dbtype)) ;
        //for sql server float type, set scale as 15, the return value of scale is 0 which is not OK for SSTable
        //6 for float, 7 for real, 8 for double, (SQL_FLOAT,SQL_REAL,SQL_DOUBLE)
        if((6 == desc[n].dbtype || 7 == desc[n].dbtype || 8 == desc[n].dbtype)
            && (0 == desc[n].scale)){
            header.setDecimals(desc[n].name, 14);
        }
        else{
            header.setDecimals(desc[n].name, desc[n].scale);
        }
        header.setMaxLength(desc[n].name, desc[n].prec);
        header.setLength(desc[n].name, desc[n].dbsize);
    }
    return true;
}

ss::uint8 odbc_driver::native_type(int type) const
{
    switch(type)
    {
    case otl_var_timestamp:
        return ss::SSTYPE_T64 ;
// 	case FIELD_TYPE_DATE:
// 	case FIELD_TYPE_TIME:
// 	case FIELD_TYPE_DATETIME:
// 	case FIELD_TYPE_YEAR:
// 		return ss::SSTYPE_T32 ;
    case otl_var_char:
        return ss::SSTYPE_STRING ;
    case otl_var_short:
        return ss::SSTYPE_INT16 ;
    case otl_var_int:
        return ss::SSTYPE_INT32 ;
    case otl_var_unsigned_int:
    case otl_var_long_int:
    case otl_var_bigint:
        return ss::SSTYPE_INT64 ;
    case otl_var_double :
        return ss::SSTYPE_DOUBLE ;
    case otl_var_float:
        return ss::SSTYPE_FLOAT ;
    default:
        return ss::SSTYPE_UNKNOWN ;
    }
}


std::string &odbc_driver::real_escape_string(std::string &from) const
{
    return from;
}

#endif
