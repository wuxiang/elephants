#include "log_module.h"

namespace Elephants
{
    CLog& CLog::instance()
    {
	    static CLog  handler;
	    return handler;
    }

    CLog::CLog()
    {
    }

    CLog::~CLog()
    {
    }

    void  CLog::init(const std::set<std::string>& module)
    {
	    std::set<std::string>::const_iterator it = module.begin();
	    while (it != module.end())
	    {
            std::string  module("Debug");
		    LOG_LEVEL   level = LOG_DEBUG;

		    std::string::size_type  pos = it->find(":");
		    if (pos != std::string::npos)
		    {
			    try
			    {
                    module = boost::lexical_cast<std::string>(it->substr(0,pos));
				    level = (LOG_LEVEL)boost::lexical_cast<int>(it->substr(pos + 1));
			    }
			    catch (const boost::bad_lexical_cast&)
			    {
			    }
		    }

		    if (mFile.find(module) == mFile.end())
		    {
			    mFile.insert(std::make_pair(module, WHandlerPtr(new WHanler(module, level))));
		    }

		    ++it;
	    }
    }


    void  CLog::wLog2File(const std::string& mod, const LOG_LEVEL lev, const std::string&  content)
    {
        std::map<std::string, WHandlerPtr>::const_iterator it = mFile.find(mod);
	    if (it != mFile.end() && it->second)
	    {
		    it->second->inputContent(lev, content);
	    }
    }


    /*****************module handle class***************************/
    CLog::WHanler::WHanler(const std::string&  mod, const LOG_LEVEL lev): level(lev), module(mod), isInit(true)
    {
        direct += module + "_Log";
    #if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
	    delimiter = '\\';
    #else
	    delimiter = '/';
    #endif

	    if (boost::filesystem::exists(direct))
	    {
		    boost::filesystem::remove_all(direct);
	    }

	    if (boost::filesystem::create_directories(direct))
	    {
		    std::time_t  curr = time(NULL);
		    filename.assign("LOG");
		    try
		    {
			    filename += boost::lexical_cast<std::string>(curr);
			    filename += ".log";
		    }
		    catch (const boost::bad_lexical_cast&)
		    {
			    isInit = false;
		    }


		    if (isInit)
		    {
			    // open file
			    std::string  absolutePath(direct + delimiter + filename);
			    iFile.open(absolutePath.c_str(), std::ios_base::out | std::ios_base::trunc);
			    if (!iFile.is_open())
			    {
				    isInit = false;
			    }
		    }
	    }
	    else
	    {
		    isInit = false;
	    }
    }

    void CLog::WHanler::inputContent(const LOG_LEVEL lev, const std::string& content)
    {
	    // print log level less than config's level
	    if ( lev > this->level)
	    {
		    return;
	    }

	    boost::lock_guard<boost::mutex>  lock(this->mtx);
	    if (this->iFile.is_open() && boost::filesystem::file_size(direct + delimiter + filename) > 1024*1024*100)
	    {
		    iFile.flush();
		    iFile.close();
		    isInit = false;
	    }

	    if (!isInit)
	    {
		    std::time_t  curr = time(NULL);
		    filename.assign("LOG");
		    try
		    {
			    filename += boost::lexical_cast<std::string>(curr);
			    filename += ".log";

			    // open file
			    std::string absolutePath(direct + delimiter + filename);
			    iFile.open(absolutePath.c_str(), std::ios_base::out | std::ios_base::trunc);
			    if (!iFile.is_open())
			    {
				    isInit = false;
			    }
			    else
			    {
				    isInit = true;
			    }
		    }
		    catch (const boost::bad_lexical_cast&)
		    {
			    isInit = false;
		    }
	    }

	    if (isInit)
	    {
		    iFile << logtime(std::time(NULL)) << ": " << content << "\n";
		    iFile.flush();
	    }

    }

    std::string  CLog::WHanler::logtime(std::time_t  sec)
    {
	    std::tm*  pTime = std::localtime(&sec);
	    std::string str;
	    try
	    {
		    str += boost::lexical_cast<std::string>(pTime->tm_year + 1900);

		    str += "-";
		    if (pTime->tm_mon + 1 < 10)
		    {
			    str += boost::lexical_cast<std::string>(0);
		    }
		    str += boost::lexical_cast<std::string>(pTime->tm_mon + 1);

		    str += "-";
		    if (pTime->tm_mday < 10)
		    {
			    str += boost::lexical_cast<std::string>(0);
		    }
		    str += boost::lexical_cast<std::string>(pTime->tm_mday);

		    str += " ";
		    if (pTime->tm_hour < 10)
		    {
			    str += boost::lexical_cast<std::string>(0);
		    }
		    str += boost::lexical_cast<std::string>(pTime->tm_hour);
		    str += ":";
		    if (pTime->tm_min < 10)
		    {
			    str += boost::lexical_cast<std::string>(0);
		    }
		    str += boost::lexical_cast<std::string>(pTime->tm_min);
		    str += ":";
		    if (pTime->tm_sec < 10)
		    {
			    str += boost::lexical_cast<std::string>(0);
		    }
		    str += boost::lexical_cast<std::string>(pTime->tm_sec);
	    } catch (boost::bad_lexical_cast&) {
		    return std::ctime(&sec);
	    }

	    return  str;
    }


    /***************************************************************************/


    /**************************global log function*****************************/
    void LOGINPUT(const std::string& mod, const LOG_LEVEL lev, const char* str,  ...)
    {
	    va_list  args;
	    va_start(args, str);

	    char  buf[4096] = { 0 };
	    vsnprintf(buf, 4096, str, args);
	    std::string  cont(buf);
	    CLog::instance().wLog2File(mod, lev, cont);

	    va_end(args);

    }

    void LOGINPUT(const std::string& mod, const LOG_LEVEL lev, const std::string& content)
    {
	    CLog::instance().wLog2File(mod, lev, content);
    }
    /************************************************************************/

}
