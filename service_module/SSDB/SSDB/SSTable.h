#ifndef SS_TABLE_H
#define SS_TABLE_H

#include <vector>
#include <string>
#include <set>
#include <boost/lexical_cast.hpp>
#include "SSDBDriver.h"

#ifdef WIN32
typedef __int64   MY_INT64;
#else
typedef int64_t   MY_INT64;
#endif

namespace ss {
const unsigned int DECIMAL_DOUBLE = 14;

using std::map;
using std::vector;
using std::string;
using std::set;
using boost::lexical_cast;
class SSTable
{
public:
    //each row of table stored by type row_type
    class row_type
    {
    public:
        row_type();
        row_type(const ss::SSTable *result, const ss::db_row &row);

        //return false if it's the prev of the first row
        bool prev() const;
        //return false if it's the next of the last row
        bool next() const;
        //the field will be set modified flag after calling function set()
        bool is_modified(const string &name) const;

        // check if the field name is existed of the row
        bool count(const string &name) const;
        // check if the field name is null or not
        bool assigned(const string &name) const;
        //same as assigned
        bool is_null(const string &name) const {return !assigned(name);}
        bool is_not_null(const string &name) const {return assigned(name);}

        // set field as null
        void set_null(const string &name);
        // delete row
        void del();

        //clear data
        void clear();

		// get field by input name
		const ss::field *get_field_by_name(const string &name) const;

        // get value of type T by input name
        template<typename T>
        T get(const string &name) const;

        // set value of type by input name
        template<typename T>
        void set(const string &name, const T &value);

        //specializations for char *
        void set(const string &name, const char *value)
        {
            set_string(name, string(value));
        }

//      const ss::db_row &data() const {return m_row;}
        // row_type is valid or not(not enough memory)
        operator bool() const {return (m_row.buffer() == NULL) ? false: true;}
        //column count
        int get_field_count() const;
        //column name
        const string &get_field_name(int cnt) const;

        //comparison < is only used by SSAlgorithm, not open for user
        bool operator<(const row_type &rhs) const { return m_row.buffer() < rhs.m_row.buffer();}

        //compare functions
        bool equal_to(const string &name, const row_type &rhs) const;
        bool not_equal_to(const string &name, const row_type &rhs) const;
        bool equal_to_null(const string &name, const row_type &rhs) const {return is_null(name);}
        bool not_equal_to_null(const string &name, const row_type &rhs) const {return is_not_null(name);}
        bool less(const string &name, const row_type &rhs) const;
        bool less_equal(const string &name, const row_type &rhs) const;
        bool greater(const string &name, const row_type &rhs) const;
        bool greater_equal(const string &name, const row_type &rhs) const;

        // commit check functions
        bool is_modified() const;
        bool is_inserted() const;
        bool is_deleted() const;

    private:
        ss::SSTable	*m_table;
        mutable ss::db_row m_row;
        bool m_new_line;

        bool operator==(const row_type &rhs) const;
        bool operator!=(const row_type &rhs) const;
        int size() const; 
        ss::field *get_field(ss::db_row *row, int cnt) const;
        ss::field *get_field(ss::db_row *row, const string &name) const;
        const ss::field_def &get_field_def(const string &name) const;
        ss::field &operator[](const string &name);
        const string get_string(const string &name) const;
        void set_string(const string &name, const string &value);
        template<typename T>
        T round(const string &name, T value, unsigned int decimals) const;
        bool double_equal(double d1, double d2, unsigned int decimal) const;
        string round_to_string(double value,unsigned int decimals) const;
        string t32_to_string(time_t time) const;
        string bit64_to_string(uint64 bit64) const;
        time_t string_to_t32(const string &time) const;
        uint64 string_to_bit64(const string &bit64) const;

        template<typename Compare>
        bool compare_to_impl(const string &name, const row_type &rhs, const Compare comp) const;

        friend class SSTable;
    }; // class row_type
public:
    static const row_type ROW_NULL;
    static const string FILE;
    class iterator;
    class const_iterator { 
    public:
        typedef ptrdiff_t difference_type;
        typedef row_type value_type;
        typedef row_type* pointer;
        typedef row_type& reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        const_iterator();
        const_iterator(const row_type &row):m_items(row){};

// 		const_iterator(const const_iterator& iter) ;
// 		const_iterator(const iterator& iter) ;
// 
// 		const_iterator& operator=(const const_iterator& iter) ;
// 		const_iterator& operator=(const iterator& iter) ;
// 
        bool operator==(const const_iterator& iter) const ;
        bool operator!=(const const_iterator& iter) const ;

        bool operator==(const iterator& iter) const ;
        bool operator!=(const iterator& iter) const ;

        const row_type &operator*() const {return m_items;};
        const row_type* operator->() const {return &m_items;};
        operator row_type() const {return m_items;};
        const_iterator& operator++();
        const_iterator operator++(int);
        const_iterator& operator--();
        const_iterator operator--(int);

        friend class SSTable ;
    protected:
        row_type m_items;
    } ;

    class iterator  : public const_iterator{ 
    public:
        iterator() ;
        iterator(const row_type &row):const_iterator(row){};

        row_type &operator*(){return m_items;};
        row_type* operator->(){return &m_items;};;
        iterator& operator++();
        iterator operator++(int);
        iterator& operator--();
        iterator operator--(int);
    } ;

    //default construct
    SSTable();
    //load table in construct directly, if may failed which can be checked by isInit()
    SSTable(const string &param, const string& sqlstr);
    //destruct
    ~SSTable();
    //load table
    bool load(const string &param, const string& sqlstr);
    //load success or not
    bool isInit() const{return m_init;}
    // table is empty
    bool empty() const {return 0 == m_result.row_count();};
    // total line num
    int size() const { return static_cast<int>(m_result.row_count());}
    // expand the table to new_size, it was used to malloc memory in advance
    bool resize(int new_size);
    // get new line
    row_type get_empty_row() const;
    // add new line at bottom
    bool push_back(const row_type &row);

    // this function opens for developer, do not use it
    bool push_back_reference(const row_type &row);

    // erase one row
    void erase(const_iterator &iter);
    void erase(iterator &iter);

    // commit all the changes to the sql db
    bool commit();
    // commit all the changes to the sql db and return the sql statments
    bool commit_ex(vector<string> *sql_statements);

    // clear all the table
    void clear();
    // clear all the content, remain table head
    void clear_data();
    //column count
    int get_field_count() const;
    //column name
    const string &get_field_name(int cnt) const;
    // check if the field name is existed of the table
    bool count(const string &name) const;

    const_iterator begin() const;
    const_iterator end() const;
    const_iterator last() const;

    iterator begin();
    iterator end();
    iterator last();

#if 0
    row_type &operator[](int n);
#endif
    const row_type &operator[](int n) const;

    bool save_to_file(const char *filename) const;

    //use strBuf.c_str() and strBuf.size() to access the Dumped binary buffer
    bool dumpForPatch(std::string& strBuf) const; 

    const string& err_msg() const {return m_errmsg;};
    int err_code() const {return m_errcode;};

    const ss::db_result& result() const {return m_result;}
    ss::db_result& result() {return m_result;}

    //operator= is allowed
    SSTable &operator=(const SSTable &rhs);

    bool is_row_modified(const row_type &row) const {return row.is_modified();}
    bool is_row_insert(const row_type &row) const {return row.is_inserted();}
    bool is_row_delete(const row_type &row) const {return row.is_deleted();}

    void clear_transaction();

    void enabled_field_name_to_lower(){m_field_name_to_lower = true;}
    void disable_field_name_to_lower(){m_field_name_to_lower = false;}
    bool is_field_name_to_lower(){return m_field_name_to_lower;}

    int primary_count() const;
    const string &get_primary(int i) const;

    // use mysql REPLACE instead of INSERT
    void set_replace_mode(bool mode){m_insert_mode = 1;}
    bool get_replace_mode(){return m_insert_mode == 1;}

    //0:INSERT mode 1: REPLACE mode 2: ON DUPLICATE KEY UPDATE 
    void set_insert_mode(int mode){m_insert_mode = mode;}
    int get_insert_mode(){return m_insert_mode;}

    //execute the sql statement direct
    static int execute(const string &param, const vector<string> &sql, string &error_msg);
private:
    bool load_from_file(const char *filename);

    void add_insert_and_modify_block(ss::block * blk);
    void erase_insert_and_modify_block(ss::block * blk);

    //string sql_modified(const ss::db_driver *driver, const string &tablename, const ss::db_row& row) const;
    //string sql_insert(const ss::db_driver *driver, const string &tablename, const ss::db_row& row) const;
    //string sql_delete(const ss::db_driver *driver, const string &tablename, const ss::db_row& row) const;


    bool				m_init;
    string				m_param;
    string				m_sqlstr;
    string				m_type;
    mutable string		m_errmsg;
    int					m_errcode;
    bool				m_field_name_to_lower;
    ss::db_result		m_result;
    int					m_insert_and_modify_id;
    map<ss::block *, int>		m_insert_and_modify_list;
    map<int, ss::block *>		m_insert_and_modify_list_timeorder;
    set<ss::block *>			m_delete_list;
    mutable vector<row_type>	*m_row_mapping_array_cache;
    int					m_insert_mode; //0:INSERT mode 1: REPLACE mode 2: ON DUPLICATE KEY UPDATE 

    //copy construct is prohibited.
    SSTable(const SSTable&);

    void clear_row_modified_flag(const ss::db_row *row);
    void clear_row_insert_flag(const ss::db_row *row);
    const ss::db_row *clear_row_delete_flag(ss::db_row *row);

    void row_mapping_array_cache() const;

};

// get value of type T by input name
template<typename T>
inline T SSTable::row_type::get(const string &name) const
{
    ss::field *field = get_field(&m_row, name);
    if(NULL == field){
        return T();
    }
    if(false == field->assigned()){
        return T();
    }
    const ss::__field_holder *holder = field->holder();
    T retstr = T();
    try{
        switch(field->type())
        {
        case ss::SSTYPE_INT8 : 
            retstr = lexical_cast<T>(holder->data.i8) ;
            break ;
        case ss::SSTYPE_INT16 : 
            retstr = lexical_cast<T>(holder->data.i16) ;
            break ;
        case ss::SSTYPE_INT32 : 
            retstr = lexical_cast<T>(holder->data.i32) ;
            break ;
        case ss::SSTYPE_INT64 : 
            retstr = lexical_cast<T>(holder->data.i64) ;
            break ;
        case ss::SSTYPE_FLOAT : 
            retstr = lexical_cast<T>(holder->data.f32) ;
            break ;
        case ss::SSTYPE_DOUBLE : 
            retstr = lexical_cast<T>(holder->data.f64) ;
            break ;
        case ss::SSTYPE_STRING :
            retstr = lexical_cast<T>(string(holder->data.str));
            break ;
        case ss::SSTYPE_RAW:
            retstr = lexical_cast<T>(string((char*)holder->data.raw,holder->length));
            break;
        case ss::SSTYPE_T32 :
            retstr = lexical_cast<T>(holder->data.t32) ;
            break ;
        case ss::SSTYPE_T64 :
            retstr = lexical_cast<T>(holder->data.t64) ;
            break ;
        case ss::SSTYPE_BIT64 :
            retstr = lexical_cast<T>(holder->data.u64) ;
            break ;
        default:
            break ;
        }
    }
    catch(boost::bad_lexical_cast&){
        retstr = T();
        //convert string type of date/time to T automatically
        if(field->type() == ss::SSTYPE_STRING){
            const ss::field_def &def = get_field_def(name);
            unsigned long length =  def.length;
            switch (length)
            {
            case 10://date(YYYY-MM-DD)
                tm timeinfo;
                memset(&timeinfo,0, sizeof(timeinfo));
                if(3 == sscanf(holder->data.str,"%04d-%02d-%02d",&timeinfo.tm_year,&timeinfo.tm_mon,&timeinfo.tm_mday)){
                    timeinfo.tm_year -= 1900;
                    timeinfo.tm_mon--;
                    time_t clock = mktime(&timeinfo);
                    if(-1 != clock){
                        try{
                            retstr = lexical_cast<T>(clock);
                        }
                        catch(...){
                            retstr = T();
                        }
                    }
                    break;
                }
            case 8://time(HH:MM:SS)
                int h,m,s;
                if(3 == sscanf(holder->data.str, "%02d:%02d:%02d",&h,&m,&s)){
                    try{
                        retstr = lexical_cast<T>(h * 3600 + m * 60 + s);
                    }
                    catch(...){
                        retstr = T();
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    return retstr;
} // T get(const string &name) const

template<>
inline string SSTable::row_type::get(const string &name) const
{
    return get_string(name);
}

// set value of type by input name
template<typename T>
inline void SSTable::row_type::set(const string &name, const T &value)
{
    ss::field *field = get_field(&m_row, name);
    if(NULL == field){
        return;
    }
    ss::__field_holder *holder = field->holder();
    uint64 oriData = holder->data.val;
    bool modified = false;
    bool assigned = true;
    const ss::field_def &def = get_field_def(name);
    try{
        switch(field->type())
        {
        case ss::SSTYPE_INT8 : 
            holder->data.i8 = static_cast<int8>(value) ;
            break ;
        case ss::SSTYPE_INT16 : 
            holder->data.i16 = lexical_cast<int16>(value) ;
            break ;
        case ss::SSTYPE_INT32 : 
            holder->data.i32 = lexical_cast<int32>(value) ;
            break ;
        case ss::SSTYPE_INT64 : 
            holder->data.i64 = lexical_cast<int64>(value) ;
            break ;
        case ss::SSTYPE_FLOAT : 
            {
                unsigned int decimals = def.decimals;
                float fval= round(name,lexical_cast<float>(value), decimals);
                if(!double_equal(holder->data.f32, fval, decimals)){
                    holder->data.f32 = fval;
                }
            }
            break ;
        case ss::SSTYPE_DOUBLE :
            {
                unsigned int decimals = def.decimals;
                double dval= round(name,lexical_cast<double>(value), decimals);
                if(!double_equal(holder->data.f64, dval, decimals)){
                    holder->data.f64 = dval;
                }
            }
            break ;
        case ss::SSTYPE_STRING :
            {
                fieldstr *str = reinterpret_cast<fieldstr *>(holder);
                string rhs = lexical_cast<string>(value);
                if(!str->assigned() || string(str->c_str()) != rhs){
                    *str = rhs.c_str();
                    modified = true;
                }
                break ;
            }
        case ss::SSTYPE_RAW :
            {
                fieldraw *raw = reinterpret_cast<fieldraw*>(holder);
                const string &rhs = lexical_cast<string>(value);
                if(!raw->assigned() || string((char*)holder->data.raw,holder->length) != rhs){
                    free(holder->data.raw);
                    if((holder->data.raw = (char *)malloc(rhs.size())) != NULL){
                        memcpy(holder->data.raw, rhs.c_str(), rhs.size());
                        holder->length = rhs.size();
                        modified = true;
                    }
                }
                break ;
            }
        case ss::SSTYPE_T32 :
            holder->data.t32 = lexical_cast<time_t>(value) ;
            break ;
        case ss::SSTYPE_T64 :
            holder->data.t64 = lexical_cast<int64>(value) ;
            break ;
        case ss::SSTYPE_BIT64 :
            holder->data.u64 = lexical_cast<uint64>(value) ;
            break;
        default:
            assigned = false;
            break ;
        }
    }
    catch(boost::bad_lexical_cast&){
        set_null(name);
        return;
    }
    if(assigned != field->assigned()){
        modified = true;
    }
    if(assigned){
        //set assigned flag
        set_bit_mask(holder->flag, 0, true);
    }

    if(m_new_line){
        //return
    }
    else if(oriData != holder->data.val || true == modified){
        //set modified flag
        set_bit_mask(field->holder()->flag, 3, true);
        // add to modify list
        m_table->add_insert_and_modify_block(m_row.get_block());
    }
    return;
} // void set(const string &name, const T &value)
//specializations for string
template<>
inline void SSTable::row_type::set(const string &name, const string &value)
{
    set_string(name, value);
}

template<typename T>
inline T SSTable::row_type::round(const string &name, T value, unsigned int decimals) const
{
    static const MY_INT64 FIX_MULTI[DECIMAL_DOUBLE + 1]={
        1,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
        10000000000,
        100000000000,
        1000000000000,
        10000000000000,
        100000000000000,
//         1000000000000000
    };
    decimals = (decimals > DECIMAL_DOUBLE ? DECIMAL_DOUBLE : decimals);
    bool minus = (value < 0 ? true : false);
    value = (value < 0 ? -value : value);
    MY_INT64 multi = FIX_MULTI[decimals];
    MY_INT64 whole_number = static_cast<MY_INT64>(value);
    value -= whole_number;
    MY_INT64 i64 = static_cast<MY_INT64>(value * multi);
    double left = value * multi - i64;
    if(left > 0.0 && (int)(left * 10000) >= 4999){
        i64++;
    }
    double dret = static_cast<double>(i64);
    dret /= multi;
    dret += whole_number;
    return static_cast<T>(minus ? -dret : dret);
}

template<typename Compare>
inline bool SSTable::row_type::compare_to_impl(const string &name, const row_type &rhs, const Compare comp) const
{
    ss::field *lfield = get_field(&m_row, name);
    ss::field *rfield = rhs.get_field(&(rhs.m_row), name);

    if(NULL == rfield || NULL == lfield){
        return false;
    }

    if(!lfield->assigned() || !rfield->assigned()){
        return false;
    }

    const ss::__field_holder *lholder = lfield->holder();
    const ss::__field_holder *rholder = rfield->holder();
    bool ret = false;

    switch(lfield->type())
    {
    case ss::SSTYPE_INT8 : 
        ret = comp(lholder->data.i8, rholder->data.i8);
        break ;
    case ss::SSTYPE_INT16 : 
        ret = comp(lholder->data.i16, rholder->data.i16);
        break ;
    case ss::SSTYPE_INT32 : 
        ret = comp(lholder->data.i32, rholder->data.i32) ;
        break ;
    case ss::SSTYPE_INT64 : 
        ret = comp(lholder->data.i64, rholder->data.i64) ;
        break ;
    case ss::SSTYPE_FLOAT : 
        ret = comp(lholder->data.f32, rholder->data.f32) ;
        break ;
    case ss::SSTYPE_DOUBLE : 
        ret = comp(lholder->data.f64, rholder->data.f64) ;
        break ;
    case ss::SSTYPE_STRING :
        ret = comp(lholder->data.str, rholder->data.str) ;
        break ;
    case ss::SSTYPE_T32 :
        ret = comp(lholder->data.t32, rholder->data.t32) ;
        break ;
    case ss::SSTYPE_T64 :
        ret = comp(lholder->data.t64, rholder->data.t64) ;
        break ;
    case ss::SSTYPE_BIT64 :
        ret = comp(lholder->data.u64, rholder->data.u64) ;
        break ;
    default:
        break ;
    }
    return ret;
}

}; //namespace
#endif
