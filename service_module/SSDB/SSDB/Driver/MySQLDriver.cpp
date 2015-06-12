
#include "Driver/MySQLDriver.h"
#include "SSLog.h"

boost::mutex mysql_driver::m_lock;
bool mysql_driver::init_ = false;

//x86 is little endian
bool check_big_endian()
{  
    union {  
        unsigned int a;  
        unsigned char b;  
    }c;  
    c.a = 1;  
    return (c.b == 1? true: false);  
}

char *htol64(char cbit[9],unsigned long max_length, char result[8])
{
    memset(result, 0, sizeof(char) * 8);
    if(0 == max_length || max_length > 8){
        return result;
    }
    memcpy(result, cbit, max_length);
    char tmp = 0;
    for(int i = 0,j = max_length - 1; i < j; i++, j--){
        tmp = result[j];
        result[j] = result[i];
        result[i] = tmp;
    }
    return result;
 }

mysql_driver::mysql_driver(const char * str) 
    : db_driver("mysql")
{
    m_lock.lock();
    if(!init_){
        if(mysql_library_init(0, NULL, NULL)){
            LOGERR("mysql_library_init::can not init mysql client") ;
        }
        else{
            init_ = true;
        }
    }
    conn_ = NULL ;
}

mysql_driver::~mysql_driver()
{
    LOGCRIT("we will close mysql connection[%s:%s:%s:%s]" , host_.c_str() , port_.c_str() , username_.c_str() , database_.c_str()) ;
    close() ;
//    mysql_library_end();
    m_lock.unlock();
}

bool mysql_driver::open(const std::string& param)
{
    split_param(param) ; //"host:port:username:password:database"

    MYSQL * mysqlInit ;
    mysqlInit = mysql_init(NULL) ;

    if(mysqlInit == NULL)
    {
        LOGERR("can not init mysql client") ;
        return false ;	
    }

    mysql_options(mysqlInit , MYSQL_SET_CHARSET_NAME , "utf8") ;

    conn_ = mysql_real_connect(mysqlInit , host_.c_str() , username_.c_str() , password_.c_str() , database_.c_str() , atoi(port_.c_str()) , NULL , 0) ;
    if(conn_ != NULL)
    {
        LOGCRIT("we have open mysql connection[%s:%s:%s:%s]" , host_.c_str() , port_.c_str() , username_.c_str() , database_.c_str()) ;
        return true ;	
    }
    else
    {
        errmsg_ = mysql_error(mysqlInit) ;
        errcode_ = mysql_errno(mysqlInit) ;
        LOGERR("we failed to open mysql connection[%s:%s:%s:%s] , errmsg[%s]" , host_.c_str() , port_.c_str() , username_.c_str() , database_.c_str() , errmsg_.c_str()) ;
        mysql_close(mysqlInit) ;
        return false ;	
    }
}

void mysql_driver::close()
{
    if(conn_ != NULL)
    {
        mysql_close(conn_) ;
//		mysql_library_end();
//		mysql_thread_end();
        conn_ = NULL ;	
    }
}

bool mysql_driver::execute(const char * sqlstr)
{
    if(conn_ == NULL || sqlstr == NULL)
        return false ;

    if(mysql_query(conn_ , sqlstr))
    {
        errmsg_ = mysql_error(conn_) ;
        errcode_ = mysql_errno(conn_) ;
        LOGERR("fail to execute[%s] , errmsg[%s]" , sqlstr , errmsg_.c_str()) ;
        return false ;
    }
    else
        return true ;
}

bool mysql_driver::get_header(ss::db_result::header& header , MYSQL_RES* res)
{
    if(res == NULL)
        return false ;

    header.clear() ;
    MYSQL_FIELD * col = NULL ;
    while((col = mysql_fetch_field(res)) != NULL)
    {
        header.add(col->name ,col->org_table, native_type(col->type)) ;
        header.setDecimals(col->name, col->decimals);
        header.setMaxLength(col->name, col->max_length);
        header.setLength(col->name, col->length);
        if(IS_PRI_KEY(col->flags)){
            header.add_primary(col->name);
        }
    }

    return true ;
}

bool mysql_driver::query(ss::db_result& result , const char * sqlstr , int limmited)
{
    result.clear() ;
    if(sqlstr == NULL)
        return false ;


    MYSQL_RES * res = NULL ;
    ss::uint64 rc = 0 ;
    ss::uint32 fc = 0 ;

    if(inner_query(sqlstr , res , rc , fc) == false || res == NULL)
        return false ;
    /*
        将结果从res 复制给 result
    */
    //初始化result的头部；
    ss::db_result::header mysql_header ;
    //格式化mysql_header的头部，这里应该直接用get_header(result.get_header(),res)，简单不少
    get_header(mysql_header , res) ;
    const ss::db_result::header& header = result.get_header() ;
    if(header.empty())  //若header不空？能保证result的头与res一样？
    {
        result.set_header(mysql_header) ; // 赋值操作符在哪里定义了？等价于result.get_header() = mysql_header？
    }
    if(header.field_count() > (int)fc)
        fc = header.field_count() ;


//    ss::field * fields = NULL ;
    //依次将res中的每一行插入result
    MYSQL_ROW row = NULL ;
    while((row = mysql_fetch_row(res)) != NULL)
    {
 

        int size = sizeof(ss::field) * fc ;
        int tlen = size ;

        ss::block * block = new ss::block(tlen) ;
        if(NULL == block){
            mysql_free_result(res);
            return false;
        }
        char * buffer = (char *)block->buffer() ;
        if(NULL == buffer){
            mysql_free_result(res);
            return false;
        }
        memset(buffer , 0 , tlen) ;
        //buffer是block中 head_+offset_，即block中有效数据的指针
        ss::__field_holder *fields = (ss::__field_holder *)buffer ;
        //依次处理result中的当前行的每一个字段，类型_field_holder;
        for(ss::uint32 idx = 0 ; idx < fc ; idx++)
        {
            //注意区分header.fields(idx)和fieldss[idx]的类型
            int type = header.fields(idx).type ;
            fields[idx].type = type ;
            fields[idx].magic = 0x5555 ;
            fields[idx].length = ss::get_type_size(type) ;
            fields[idx].data.val = 0 ;

// 			ss::set_bit_mask(fields[idx].flag , 1 , true) ;		//²»ÄÜ¸³Öµ
// 			ss::set_bit_mask(fields[idx].flag , 2 , true) ;		//²»ÄÜ×Ô¶¯ÊÍ·Å

            if(row[idx] == NULL)
                continue ;

            time_t t = 0;
            int msec = 0 ;
            bool assigned = true ;
            //如何保证result的头与res的头结构一致？
            switch(type)
            {
            case ss::SSTYPE_INT8 : 
                fields[idx].data.i8 = (ss::int8)atoi(row[idx]) ;
                break ;
            case ss::SSTYPE_INT16 : 
                fields[idx].data.i16 = (ss::int16)atoi(row[idx]) ;
                break ;
            case ss::SSTYPE_INT32 : 
                fields[idx].data.i32 = (ss::int32)atoi(row[idx]) ;
                break ;
            case ss::SSTYPE_INT64 : 
                fields[idx].data.i64 = (ss::int64)atoi64(row[idx]) ;
                break ;
            case ss::SSTYPE_FLOAT : 
                fields[idx].data.f32 = (float)atof(row[idx]) ;
                break ;
            case ss::SSTYPE_DOUBLE : 
                fields[idx].data.f64 = (double)atof(row[idx]) ;
                break ;
            case ss::SSTYPE_STRING :
                {
                    int strsize = ::strlen(row[idx]) + 1;
                    char * pchar = (char *)malloc(strsize);
                    if(NULL == pchar){
                        assigned = false ;
                    }
                    else{
                        memcpy(pchar , row[idx] , strsize) ;
                        fields[idx].data.str = pchar;
                        fields[idx].length = strsize;
                    }
                    break ;
                }
            case ss::SSTYPE_RAW :
                {
                    unsigned long *field_len = mysql_fetch_lengths(res);
                    char * pch = (char *)malloc(field_len[idx]);
                    if(NULL == pch){
                        assigned = false;
                    }
                    else{
                        memcpy(pch, row[idx], field_len[idx]);
                        fields[idx].data.raw = pch;
                        fields[idx].length = (unsigned int)field_len[idx];
                    }
                    break;
                }
            case ss::SSTYPE_T32 :
                if(ss::str2time(row[idx] , t , msec) == false)
                    assigned = false ;
                else
                {
                    fields[idx].data.t32 = t ;
                }
                break ;
            case ss::SSTYPE_T64 :
                if(ss::str2time(row[idx] , t , msec) == false)
                    assigned = false ;
                else
                {
                    fields[idx].data.t64 = (ss::int32)t ;
                    fields[idx].data.t64 = (fields[idx].data.t64 * 1000) ;
                    fields[idx].data.t64 += msec ;
                }
                break ;
            case ss::SSTYPE_BIT64 :
                {
                    char result[8];
                    fields[idx].data.u64 = *(ss::uint64 *)htol64(row[idx],header.fields(idx).max_length, result);
                    break ;
                }
            default:
                assigned = false ;
                break ;
            }		

            if(assigned == true)
            {
                ss::set_bit_mask(fields[idx].flag , 0 , true) ;
            }
        }//endif  for ....

        block->length(tlen) ;
        //db_row(block)中是将block增加一个定值后用来初始化db_row的block_；
        ss::db_row * db_row = new ss::db_row(block) ;
        if(NULL == db_row){
            mysql_free_result(res);
            return false;
        }
        //和上面db_row(block)重复？
        db_row->attach(block) ;

        result.add_row(db_row) ;
    }//end if while ...

    mysql_free_result(res);
    return true ;
}
bool mysql_driver::print_mysql_row(ss::db_result::header& header , int index , MYSQL_ROW row , char *  buffer , int buflen) 
{
    int field_count = header.field_count() ;
    int len = 0 ;
    len += sprintf(buffer , "INDEX[%d] " , index) ;
    for(int idx = 0 ; idx < field_count ; idx++)
    {
        ss::int8 type = header.fields(idx).type ;
        len += sprintf(buffer + len , "NAME[%s] TYPE[%s] VALUE[%s] LENGTH[%d]" , 
            header.fields(idx).name.c_str() , ss::get_type_desc(type) , row[idx] , (row[idx] == NULL) ? 0 : ((int)::strlen(row[idx]) + 1) ) ;
    }

    return true ;
}

ss::uint8 mysql_driver::native_type(enum_field_types type)
{
    switch(type)
    {
     case FIELD_TYPE_TIMESTAMP:
         return ss::SSTYPE_T64 ;
     case FIELD_TYPE_DATE:
     case FIELD_TYPE_TIME:
     case FIELD_TYPE_DATETIME:
     case FIELD_TYPE_YEAR:
         return ss::SSTYPE_T32 ;
     case FIELD_TYPE_STRING:
     case FIELD_TYPE_VAR_STRING:
     case FIELD_TYPE_SET:
     case FIELD_TYPE_NULL:
         return ss::SSTYPE_STRING ;
     case FIELD_TYPE_TINY_BLOB:
     case FIELD_TYPE_MEDIUM_BLOB: 
     case FIELD_TYPE_LONG_BLOB:   
     case FIELD_TYPE_BLOB:
         return ss::SSTYPE_RAW;
     case FIELD_TYPE_TINY:
         return ss::SSTYPE_INT8 ;
     case FIELD_TYPE_SHORT:
         return ss::SSTYPE_INT16 ;
     case FIELD_TYPE_LONG:
     case FIELD_TYPE_INT24:
     case FIELD_TYPE_ENUM:
         return ss::SSTYPE_INT32 ;
     case FIELD_TYPE_LONGLONG:
         return ss::SSTYPE_INT64 ;
     case FIELD_TYPE_BIT:
         return ss::SSTYPE_BIT64 ;
     case FIELD_TYPE_DECIMAL:
     case FIELD_TYPE_DOUBLE:
     case FIELD_TYPE_NEWDECIMAL :
         return ss::SSTYPE_DOUBLE ;
     case FIELD_TYPE_FLOAT:
         return ss::SSTYPE_FLOAT ;
     default:
         return ss::SSTYPE_UNKNOWN ;
    }
}

bool mysql_driver::inner_query(const char * sqlstr , MYSQL_RES *& result ,ss::uint64& rc , ss::uint32& fc)
{
    if(conn_ == NULL || sqlstr == NULL)
    {
        errmsg_ = "conn_ or sql is empty" ;
        errcode_ = -1 ;
        return false ;
    }

    if(mysql_query(conn_ , sqlstr))
    {
        errcode_ = mysql_errno(conn_) ;
        const char * errstr = mysql_error(conn_) ;
        if(errstr == NULL)
            errmsg_ = "can not execute sqlstr" ;
        else
            errmsg_ = errstr ;

        return false ;
    }


    if((result = mysql_store_result(conn_)) == NULL)
    {
        errcode_ = mysql_errno(conn_) ;
        errmsg_ = "can not store result" ;
        return false ;
    }

    rc = mysql_num_rows(result) ;
    fc = mysql_num_fields(result) ;

    return true ;
}


std::string &mysql_driver::real_escape_string(std::string &from) const
{
    char *to = (char *)malloc(from.size() * 2 + 1);
    if(NULL == to){
        return from;
    }
    mysql_real_escape_string(conn_, to, from.c_str(), from.size());
    from = to;
    free(to);
    return from;
}

