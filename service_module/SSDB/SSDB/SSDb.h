#ifndef SS_DB_H
#define SS_DB_H

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

namespace ss{
class SSTable;
class SSDb
{
public:
    SSDb();
    SSDb(const string &param, const string &db);
    ~SSDb();
    //param: ip:port:username:passwd
    //db: db name
    bool connect(const string &param, const string &db);
    
    //release all the table and clear the m_tables
    void clear();

    //same as "select * from tablename"
    bool load(const string &tablename);

    //load all the table as "select * from [tables]"
    //if db size is large, memory will not enough
    bool load_all();

    //table name 1
    //table name 2
    //...
    //table name n
    vector<string> get_table_list() const;

    //if table is not loaded, load it and return SSTable *
    //if table is loaded, just return SSTable *
    //if table is found in db, return NULL
    SSTable *find(const string &tablename);
    //const version of find
    const SSTable *find(const string &tablename) const;

    const SSTable *insert(const string &tablename, const SSTable &table);
    const SSTable *insert(const string &tablename, const string &param, const string& sqlstr);
    void erase(const string &tablename);

    //save all the loaded tables to files in "foldername"
    bool save_to_folder(const char *foldername) const;

    //save all tables include unloaded to files in "foldername"
    bool save_all_to_folder(const char *foldername) const;

    static const string FOLDER;

private:
    //copy construct is prohibited
    SSDb(const SSDb &);
    //operator= is prohibited
    SSDb &operator=(const SSDb &);

    map<string, SSTable *> m_tables;
    string m_param;
    string m_db;

    bool create_folder(const char *foldername, string &folderpath) const;
    bool load_table(const string &tablename, SSTable *table) const;
};

}
#endif
