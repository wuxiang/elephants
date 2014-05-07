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
	    EMERG		= 0,   /* system is unusable */
	    ALERT		= 1,   /* action must be taken immediately */
	    CRIT		= 2,   /* critical conditions */
	    ERR			= 3,   /* error conditions */
	    WARNING		= 4,   /* warning conditions */
	    NOTICE		= 5,   /* normal but significant condition */
	    INFO		= 6,   /* informational */
	    DEBUG		= 7,   /* debug-level messages */
    };

    void DA_LOG(const std::string& mod, const LOG_LEVEL lev, const char*  str,  ...);
    void DA_LOG(const std::string& mod, const LOG_LEVEL lev, const std::string& content);
    void DA_ERRLOG(const char*  fmt,  ...);
    void DA_ERRLOG(const std::string& content);


    class CLog
    {
    public:
	    static CLog& instance();
	    bool  init(const std::vector<std::string>& module);
        void  wLog2File(const std::string& mod, const LOG_LEVEL lev, const std::string&  content);

        static LOG_LEVEL  reflex(const std::string& level);
        static std::string today();


    private:
	    CLog();
	    ~CLog();
	    CLog(const CLog&);
	    const CLog& operator=(const CLog&);
        void inputHandler(const std::string& name,  const std::string& level);

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
            std::string  timestamp;
		    boost::mutex  mtx;
		    std::fstream  iFile;
	    };

	    typedef boost::shared_ptr<WHanler>   WHandlerPtr;

    private:
        boost::shared_mutex   m_mtx;
        std::map<std::string, WHandlerPtr>  mFile;
    };

}

#define DA_LOG(...) Elephants::DA_LOG(__VA_ARGS__)
#define DA_ERRLOG(...) Elephants::DA_ERRLOG(__VA_ARGS__)

#endif //_CLOG_H_


