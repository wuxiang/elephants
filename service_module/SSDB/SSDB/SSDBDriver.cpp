
#include "SSDBDriver.h"
#include "Driver/MySQLDriver.h"
#if defined(ODBC_DRIVER)
#include "Driver/ODBCDriver.h"
#endif
#if defined(OCI_DRIVER)
#include "oci/OCIDriver.h"
#endif

namespace ss {

    db_driver::db_driver(const char * str)
    {
        errcode_ = 0 ;
    }

    db_driver::~db_driver()
    {
        close() ;
    }

    void db_driver::split_param(const std::string& param)
    {
        const char * str = param.c_str() ;
        int count = 0 , slen = 0;
        char strbuf[256] = {'\0'} ;
        char c = 0 ;

        do{
            c = *str ;
            if(c == '\0' || c == ':')
            {
                strbuf[slen] = '\0' ;
                switch(count)
                {
                case 0 : host_ = strbuf ; break ;
                case 1 : port_ = strbuf ; break ;
                case 2 : username_ = strbuf ; break ;
                case 3 : password_ = strbuf ; break ;
                case 4 : database_ = strbuf ; break ;
                }		

                count++ ;
                slen = 0 ;
                strbuf[0] = '\0' ;

                if(c == ':')
                {
                    str++ ;
                }
            }
            else
            {
                strbuf[slen] = c ;
                slen++ ;
                str++ ;
            }	
        }while(c != '\0') ;	
    }

    bool db_driver::open(const std::string& param)
    {
        return false ;
    }

    void db_driver::close()
    {
        return ;
    }

    bool db_driver::execute(const char * sqlstr)
    {
        return false ;
    }

    bool db_driver::query(db_result& result , const char * sqlstr , int limmited) 
    {
        return false ;
    }

    bool db_driver::begin()
    {
        return execute("START TRANSACTION") ;
    }

    bool db_driver::rollback()
    {
        return execute("ROLLBACK") ;
    }

    bool db_driver::commit()
    {
        return execute("COMMIT") ;
    }

    std::string &db_driver::real_escape_string(std::string &from) const
    {
        return from;
    }

    db_driver * db_driver::create(const std::string& name)
    {
        #if defined(MYSQL_DRIVER)
        if(name == "mysql")
            return new mysql_driver(NULL) ;
        #endif
        #if defined(ODBC_DRIVER)
        if(name == "odbc")
                return new odbc_driver(NULL) ;
        #endif

        #if defined(OCI_DRIVER)
        if(name == "oci")
                return new oci_dirver();
        #endif
        return NULL ;
    }

    db_driver * db_driver::create()
    {
        #if defined(MYSQL_DRIVER)
        return new mysql_driver(NULL) ;
        #endif

        #if defined(OCI_DRIVER)
        return new oci_dirver();
        #endif
    }
}

