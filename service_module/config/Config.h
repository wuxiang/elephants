/* 
   Config.h
   Copyright (C) 

*/

#ifndef DA_CONFIG_H
#define DA_CONFIG_H

#include <string>
#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <time.h>
#include <map>

using std::string;
using std::map;
using std::vector;
#define AUTOSYNC(a) boost::mutex::scoped_lock lock(a)
class Config
{
public:
    static Config* Instance();
    bool fail(){return !init_;};

    const string& get(const string &key);
    const vector<string>& get_vector(const string &key);
    int count(const string &key);

    string today();
private:
    Config();
    virtual ~Config();
    Config(const Config&);

    void run();
    void init();
    void clear();

    static Config* instance_;

    bool init_;
    map<string, vector<string> > m_setting;

    static boost::thread thread_;
    boost::mutex mutex_;
    time_t t_;

    string empty_string; //for false return value
    static vector<string> null_vector;
};
#endif

