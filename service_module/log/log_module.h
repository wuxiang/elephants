#ifndef _CLOG_H_
#define _CLOG_H_
#include <fstream>
#include <set>
#include <map>
#include <string>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <cstdarg>
#include <cstdio>

namespace Elephants
{
    enum LOG_LEVEL
    {
	    LOG_EMERG		= 0,   /* system is unusable */
	    LOG_ALERT		= 1,   /* action must be taken immediately */
	    LOG_CRIT		= 2,   /* critical conditions */
	    LOG_ERR			= 3,   /* error conditions */
	    LOG_WARNING		= 4,   /* warning conditions */
	    LOG_NOTICE		= 5,   /* normal but significant condition */
	    LOG_INFO		= 6,   /* informational */
	    LOG_DEBUG		= 7,   /* debug-level messages */
    };

    enum LOG_MODULE
    {
	    LOG_MSG = 100,
	    LOG_PROCESS_ORDER = 101,
	    LOG_STATISTICS = 102,
    };

    void LOGINPUT(const LOG_MODULE mod, const LOG_LEVEL lev, const char*  str,  ...);
    void LOGINPUT(const LOG_MODULE mod, const LOG_LEVEL lev, const std::string& content);

    class CLog
    {
    public:
	    static CLog& instance();
	    void  init(const std::set<std::string>& module);
        void  wLog2File(const std::string& mod, const LOG_LEVEL lev, const std::string&  content);

    private:
	    CLog();
	    ~CLog();
	    CLog(const CLog&);
	    const CLog& operator=(const CLog&);

    protected:
	    class  WHanler
	    {
	    public:
            WHanler(const std::string&  mod, const LOG_LEVEL lev);
		    void inputContent(const LOG_LEVEL lev, const std::string& content);

	    private:
		    std::string  logtime(std::time_t  sec);
		    WHanler(const WHanler&);

	    private:
		    bool  isInit;
		    LOG_LEVEL   level;
            std::string   module;

		    std::string  direct;  // 目录
		    std::string  delimiter; //分隔符
		    std::string  filename;  //文件名
		    boost::mutex  mtx;
		    std::fstream  iFile;
	    };

	    typedef boost::shared_ptr<WHanler>   WHandlerPtr;

    private:
        std::map<std::string, WHandlerPtr>  mFile;
    };

}
#endif //_CLOG_H_


