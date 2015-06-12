#include "SSDb.h"
#include "SSTable.h"
#include <boost/filesystem.hpp>
#include <iostream>
using namespace boost::filesystem;
using namespace std;

namespace ss{

const char EXT[6] = ".ssdb";
const string SSDb::FOLDER = "<FOLDER>";

SSDb::SSDb()
{

}

SSDb::SSDb(const string &param, const string &db)
{
    connect(param, db);
}

SSDb::~SSDb()
{
    clear();
}

bool
SSDb::connect(const string &param, const string &db)
{
    if(param == FOLDER){
        path target_path(db);
        target_path = system_complete(target_path);
        if(!is_directory(target_path)){
            return false;
        }

        clear();
        directory_iterator iter(target_path);
        directory_iterator iter_end;
        for(; iter != iter_end; ++iter){
            if(EXT == iter->path().extension()){
                string filename = basename(iter->path());
                m_tables[filename] = NULL;
            }
        }
        m_param = param;
        m_db = target_path.string();
    }
    else{
        SSTable tables;
        if(false == tables.load(param + ":" + db, "SHOW TABLES")){
            return false;
        }
        clear();
        string col_name = "Tables_in_" + db;
        for(SSTable::const_iterator iter = tables.begin(); iter != tables.end(); ++iter){
            m_tables[iter->get<string>(col_name)] = NULL;
        }
        m_param = param;
        m_db = db;
    }
    return true;
}

void
SSDb::clear()
{
    for(map<string, SSTable *>::iterator iter = m_tables.begin(); iter != m_tables.end(); ++iter){
        if(NULL != iter->second){
            delete iter->second;
            iter->second = NULL;
        }
    }
    m_tables.clear();
    m_param.clear();
    m_db.clear();
}

bool
SSDb::load(const string &tablename)
{
    //no this table
    if(0 == m_tables.count(tablename)){
        return false;
    }
    //already load
    if(NULL != m_tables[tablename]){
        return true;
    }
    //load table
    SSTable *table = new SSTable;
    if(NULL == table){
        return false;
    }
    if(false == load_table(tablename, table)){
        return false;
    }
    m_tables[tablename] = table;
    return true;
}

bool
SSDb::load_all()
{
    for(map<string, SSTable *>::const_iterator iter = m_tables.begin(); iter != m_tables.end(); ++iter){
        if(NULL == iter->second){
            if(false == load(iter->first)){
                return false;
            }
        }
    }
    return true;
}

vector<string>
SSDb::get_table_list() const
{
    vector<string> list;
    for(map<string, SSTable *>::const_iterator iter = m_tables.begin(); iter != m_tables.end(); ++iter){
        list.push_back(iter->first);
    }
    return list;
}

SSTable *
SSDb::find(const string &tablename)
{
    if(false == load(tablename)){
        return NULL;
    }
    return m_tables[tablename];
}

const SSTable *
SSDb::find(const string &tablename) const
{
    return const_cast<SSDb *>(this)->find(tablename);
}

const SSTable *
SSDb::insert(const string &tablename, const SSTable &table)
{
    if(m_tables.count(tablename) && NULL !=m_tables[tablename]){
        *m_tables[tablename] = table;
    }
    else{
        //load table
        SSTable *ptable = new SSTable;
        if(NULL == ptable){
            return NULL;
        }
        *ptable = table;
        m_tables[tablename] = ptable;
    }
    return m_tables[tablename];
}

const SSTable *
SSDb::insert(const string &tablename, const string &param, const string& sqlstr)
{
    SSTable *ptable = NULL;
    if(m_tables.count(tablename) && NULL !=m_tables[tablename]){
        ptable = m_tables[tablename];
    }
    else{
        ptable = new SSTable;
        if(NULL == ptable){
            return NULL;
        }
        m_tables[tablename] = ptable;
    }
    if (false == ptable->load(param, sqlstr)){
        delete ptable;
        m_tables[tablename] = NULL;
        return NULL;
    }
    return m_tables[tablename];
}

void
SSDb::erase(const string &tablename)
{
    if(m_tables.count(tablename)){
        delete m_tables[tablename];
        m_tables.erase(tablename);
    }
}


//save all the loaded tables to files in "foldername"
bool
SSDb::save_to_folder(const char *foldername) const
{
    string folder_path;
    if(false == create_folder(foldername, folder_path)){
        return false;
    }

    for(map<string, SSTable *>::const_iterator iter = m_tables.begin(); iter != m_tables.end(); ++iter){
        if(iter->second != NULL){
            path file(folder_path);
            file /= (iter->first + EXT);
            if(false == iter->second->save_to_file(file.string().c_str())){
                return false;
            }
        }
    }
    return true;
}

//save all tables include unloaded to files in "foldername"
bool
SSDb::save_all_to_folder(const char *foldername) const
{
    string folder_path;
    if(false == create_folder(foldername, folder_path)){
        return false;
    }

    for(map<string, SSTable *>::const_iterator iter = m_tables.begin(); iter != m_tables.end(); ++iter){
        path file(folder_path);
        file /= (iter->first + EXT);
        if(iter->second != NULL){
            if(false == iter->second->save_to_file(file.string().c_str())){
                return false;
            }
        }
        else{
            SSTable *table = new SSTable;
            if(NULL == table){
                return false;
            }
            if(false == load_table(iter->first, table)){
                delete table;
                return false;
            }
            table->save_to_file(file.string().c_str());
            delete table;
        }
    }
    return true;
}

bool
SSDb::create_folder(const char *foldername, string &folderpath) const
{
    path target_path(foldername);
    target_path = system_complete(target_path);
    boost::system::error_code ec;
    create_directories(target_path,ec);
    if(ec){
        cout << boost::system::system_error(ec).what() << endl;
        cout << target_path << endl;
        return false;
    }
    folderpath = target_path.string();
    return true;
}

bool
SSDb::load_table(const string &tablename, SSTable *table) const
{
    if(FOLDER == m_param){
        path filename(m_db);
        filename /= (tablename + EXT);
        if(false == table->load(ss::SSTable::FILE, filename.string())){
            return false;
        }
    }
    else{
        if(false == table->load(m_param + ":" + m_db, "SELECT * from " + tablename)){
            return false;
        }
    }
    return true;
}


}//namespace ss
