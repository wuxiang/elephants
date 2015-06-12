#include "SSAlgorithm.h"
#include <algorithm>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <iterator>
#include <SSMyLog.h>

using std::vector;
using std::map;
using std::string;
using std::set;
using std::insert_iterator;

namespace ss{

bool copy_table(const SSTable &src_table, SSTable &dst_table){

    int dst_pk_cnt = dst_table.primary_count();
    if(0 == dst_pk_cnt){
        DA_ERRLOG("Copy Error:no primary key in dst table!");
        return false;
    }
    map<string,int> dst_pk_map;     // primary key fields of dst table;     
    for(int k = 0; k < dst_pk_cnt; ++k){
        const string dst_pk_field = dst_table.get_primary(k);
        // primary key fields of dst table doesn't exist in src table,copy end;
        if(false == src_table.count(dst_pk_field)){
            DA_ERRLOG("Copy Error:no primary key of dst table exist in src table!");
            return false;  
        }
        ++dst_pk_map[dst_pk_field];
    }
    //figure out the number of same fields except primary key of dst table;
    int src_field_cnt ,dst_field_cnt;
    src_field_cnt = src_table.get_field_count();
    dst_field_cnt = dst_table.get_field_count();
    map<string,int> field_map;    //all fields in both table except primary keys fields of dst;
    int i,j;
    i = j = 0;
    while(i < src_field_cnt || j < dst_field_cnt){
        string field_name;
        if(i < src_field_cnt){
            field_name = src_table.get_field_name(i);
            if(!dst_pk_map.count(field_name)){
                ++field_map[field_name];               
            }
            ++i;
        }
        if(j < dst_field_cnt){              
            field_name = dst_table.get_field_name(j);
            if(!dst_pk_map.count(field_name)){
                ++field_map[field_name];
            }
            ++j;
        }
    }
    int same_field_cnt = 0;	// the number of same fields except primary key of dst table;
    map<string,int>::const_iterator map_it = field_map.begin();
    while (map_it != field_map.end()){
        if(2 == map_it->second){
            ++same_field_cnt;
        }
        ++map_it;
    }

    SSTable::row_type src_row,dst_row;
    src_row = src_table.begin();
    dst_row = dst_table.begin();
    vector<ss::SSTable::row_type> temp_vec;  // temp vector of pointers of the rows to be inserted into dst;  
   
    while(src_row && dst_row){
        string dst_pk_str,src_pk_str;       // link values of all primary key field in the row as a string; 
        for(map<string,int>::iterator map_it = dst_pk_map.begin();
            map_it != dst_pk_map.end(); ++map_it){
            src_pk_str += src_row.get<string>(map_it->first);
            dst_pk_str += dst_row.get<string>(map_it->first);
        }
        //same primary key values 
        if (dst_pk_str == src_pk_str){
            //no same fields except primary keys;
            if(0 == same_field_cnt){
                dst_row.next();
                src_row.next();
                continue;
            }
            for(int cnt=0; cnt < src_field_cnt; ++cnt){
                string name = src_row.get_field_name(cnt);
                if(dst_row.count(name)){
                    //update value
                    if(src_row.assigned(name) && !dst_row.assigned(name)){
                        dst_row.set(name,src_row.get<string>(name));
                    }
                    else if(src_row.assigned(name) && dst_row.assigned(name)
                            && dst_row.get<string>(name) != src_row.get<string>(name)){
                        dst_row.set(name,src_row.get<string>(name));
                    }
                    else if( !src_row.assigned(name) && dst_row.assigned(name)){
                        dst_row.set_null(name);
                    }
                }
            }
            dst_row.next();
            src_row.next();
        }
        //primary key value of dst row is smaller;
        else if (dst_pk_str < src_pk_str){
            dst_row.del();
            dst_row.next();
        }
        //primary key value of dst row is bigger;
        else{
            temp_vec.push_back(src_row);
            src_row.next();
        }
    }
    //delete rest rows of dst
    while(dst_row){
        dst_row.del();
        dst_row.next();
    }
    //copy rest rows of src to temp_vec
    while(src_row){
        temp_vec.push_back(src_row);
        src_row.next();
    }
    //insert rows of temp_vec into dst
    for(vector<SSTable::row_type>::iterator iter = temp_vec.begin(); iter != temp_vec.end(); ++iter){
        SSTable::row_type empty_row = dst_table.get_empty_row();
        for(int cnt = 0; cnt < src_field_cnt; ++cnt){
            string name = src_row.get_field_name(cnt);
            if(empty_row.count(name) && (*iter).assigned(name)){
                empty_row.set(name, (*iter).get<string>(name));
            }
        }
        dst_table.push_back(empty_row);
    }
    return true;
}

int search_equal(const SSTable &src_table, SSTable &result, const string &name, const string &equal_value)
{
    return search_by_compare_function(src_table, result, name, equal_value, &ss::SSTable::row_type::equal_to);
}

int search_not_equal(const SSTable &src_table, SSTable &result, const string &name, const string &equal_value)
{
    return search_by_compare_function(src_table, result, name, equal_value, &ss::SSTable::row_type::not_equal_to);
}

int search_is_null(const SSTable &src_table, SSTable &result, const string &name)
{
    return search_by_compare_function(src_table, result, name, string(), &ss::SSTable::row_type::equal_to_null);
}

int search_is_not_null(const SSTable &src_table, SSTable &result, const string &name)
{
    return search_by_compare_function(src_table, result, name, string(), &ss::SSTable::row_type::not_equal_to_null);
}

int search_less(const SSTable &src_table, SSTable &result, const string &name, const string &value)
{
    return search_by_compare_function(src_table, result, name, value, &ss::SSTable::row_type::less);
}

int search_less_equal(const SSTable &src_table, SSTable &result, const string &name, const string &value)
{
    return search_by_compare_function(src_table, result, name, value, &ss::SSTable::row_type::less_equal);
}

int search_greater(const SSTable &src_table, SSTable &result, const string &name, const string &value)
{
    return search_by_compare_function(src_table, result, name, value, &ss::SSTable::row_type::greater);
}

int search_greater_equal(const SSTable &src_table, SSTable &result, const string &name, const string &value)
{
    return search_by_compare_function(src_table, result, name, value, &ss::SSTable::row_type::greater_equal);
}

int table_union(const SSTable &input_a, const SSTable &input_b, SSTable &output)
{
    //check if table field names are same.(It's not hard check, only check field count)
    if(input_a.get_field_count()!= input_b.get_field_count()){
        return 0;
    }

    set<SSTable::row_type> set_a(input_a.begin(),input_a.end());
    set<SSTable::row_type> set_b(input_b.begin(),input_b.end());
    set<SSTable::row_type> set_output;
    std::set_union(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(), insert_iterator<set<SSTable::row_type> >(set_output, set_output.begin()));
    if(set_output.size()){
        output.clear();
        output.result().set_header(input_a.result().get_header());
        for(set<SSTable::row_type>::const_iterator iter = set_output.begin(); iter != set_output.end(); ++iter){
            output.push_back_reference(*iter);
        }
    }
    return output.size();
}

int table_intersection(const SSTable &input_a, const SSTable &input_b, SSTable &output)
{
    //check if table field names are same.(It's not hard check, only check field count)
    if(input_a.get_field_count()!= input_b.get_field_count()){
        return 0;
    }
    
    set<SSTable::row_type> set_a(input_a.begin(),input_a.end());
    set<SSTable::row_type> set_b(input_b.begin(),input_b.end());
    set<SSTable::row_type> set_output;
    std::set_intersection(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(), insert_iterator<set<SSTable::row_type> >(set_output, set_output.begin()));
    if(set_output.size()){
        output.clear();
        output.result().set_header(input_a.result().get_header());
        for(set<SSTable::row_type>::const_iterator iter = set_output.begin(); iter != set_output.end(); ++iter){
            output.push_back_reference(*iter);
        }
    }
    return output.size();
}

//for developer
int search_by_compare_function(const SSTable &src_table, SSTable &result, const string &name, const string &equal_value, func_comp comp)
{
    if(false == src_table.count(name)){
        return 0;
    }
    result.clear();
    SSTable::row_type row = src_table.get_empty_row();
    row.set(name,equal_value);
    string value = row.get<string>(name);
    result.result().set_header(src_table.result().get_header());
    for(SSTable::const_iterator iter = src_table.begin(); iter != src_table.end(); ++iter){
        if(((*iter).*comp)(name, row)){
            result.push_back_reference(*iter);
        }
    }
    return result.size();
}

//copy table head, dst table will be cleared.
void copy_table_head(ss::SSTable &dst, const ss::SSTable &src)
{
    dst.clear();
    dst.result().set_header(src.result().get_header());
}

}
