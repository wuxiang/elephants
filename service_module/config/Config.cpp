/* 
   Config.h
   Copyright (C) 
    

*/


#include "Config.h"
#include "Log.h"
//#include "BasicType.h"
#include <fstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "boost/algorithm/string.hpp"
#include <boost/filesystem/operations.hpp>
#include <time.h>
using namespace std;

#define CHECK_INTERVAL 5 //5 sec
#define DEFAULT_SERVER_NUM 5
#define DEFAULT_TIMEOUT 30 //30 sec
#define FILE_NAME "setting.ini"
Config *Config::instance_ = NULL;
boost::thread Config::thread_;
vector<string> Config::null_vector;

Config*
Config::Instance()
{
    if(NULL == instance_){
        instance_ = new Config;
        instance_->init();
//         thread_ = boost::thread(boost::bind(&Config::run,instance_));
    }
    return instance_;
}

Config::~Config()
{
    if(NULL != instance_){
        delete instance_;
    }
}


Config::Config():
                init_(false),
                t_(0)
{
}

void
Config::run()
{
    while(1){
        boost::this_thread::sleep(boost::posix_time::seconds(CHECK_INTERVAL));
        init();
    }
}

void
Config::init()
{
    AUTOSYNC(mutex_);
    boost::filesystem::path path(FILE_NAME);
    try{
        boost::filesystem::path curpath = boost::filesystem::current_path();
        if(t_ == boost::filesystem::last_write_time( path )){
            return;
        }
        else{
            t_ = boost::filesystem::last_write_time( path );
        }
    }
    catch(boost::filesystem::filesystem_error e) { 
        DA_ERRLOG("%s: %s", __FUNCTION__, e.what());
    }    

    ifstream in(FILE_NAME,ios::in);
    if(in.fail()){
        DA_ERRLOG("[%s]%s open fail",__FUNCTION__, FILE_NAME);
        init_ = false;
        return;
    }

    try{
        clear();
        std::string line;
        while(!in.eof()){
            std::getline(in,line);
            std::string::size_type pos = line.find("=");
            if(pos == std::string::npos){
                continue;
            }
            std::string title = line.substr(0,pos);
            std::string content = line.substr(pos + 1);
            boost::algorithm::trim(title);
            boost::algorithm::trim(content);

            m_setting[title].push_back(content);
        }
    }
    catch(boost::bad_lexical_cast& e){
        DA_ERRLOG("[%s]%s",__FUNCTION__,e.what());
        init_=false;
    }
    init_ = true;
    DA_LOG("Config.ini updated");
    return;
}

void
Config::clear()
{
    m_setting.clear();
}

const string&
Config::get(const string &key)
{
    if(m_setting.count(key)){
        return m_setting[key][0];
    }
    return empty_string;
}

const vector<string>&
Config::get_vector(const string &key)
{
    AUTOSYNC(mutex_);
    if(m_setting.count(key)){
        return m_setting[key];
    }
    return null_vector;
}

int
Config::count(const string &key)
{
    if(0 == m_setting.count(key)){
        return 0;
    }
    return m_setting[key].size();
}

string
Config::today()
{
    time_t timeval;
    const tm    *timeinfo;

    timeval = time(NULL);  
    timeinfo = localtime(&timeval);
    char time_val[50];
    sprintf(time_val,"%04d%02d%02d",
        timeinfo->tm_year + 1900,
        timeinfo->tm_mon + 1,
        timeinfo->tm_mday
        );
    return string(time_val);
}

