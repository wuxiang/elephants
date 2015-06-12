#ifndef __SS_ALGORITHM_H
#define __SS_ALGORITHM_H 1

#include "SSTable.h"

namespace ss {
/**
 * bool copy_table(const SSTable &src_table, SSTable &dst_table)
 * dst_table's field name and field count may not be same as src_table.
 * It will find the same fields and copy the values.
 * This function is ussually used to merge src_table to dst_table
 * @param	src_table[IN]: source table
 * @param	dst_table[OUT]: target table
 * @return	always true
 * @see
 * @note	dst_table must be initialized before invoking the function 
 */
bool copy_table(const SSTable &src_table, SSTable &dst_table);

/**
 * int search_equal(const SSTable &src_table, SSTable &result, const string &name, const string &equal_value)
 * it's like sql statement "select * from src_table where name = equal_value"
 * @param	src_table[IN]: source table
 * @param	result[OUT]: target table
 * @param	name[IN]: field name
 * @param	equal_value[IN]: search string
 * @return	found line count
 * @see
 * @note	data in result is the reference of the src_table
 */
int search_equal(const SSTable &src_table, SSTable &result, const string &name, const string &equal_value);

/**
 * int search_not_equal(const SSTable &src_table, SSTable &result, const string &name, const string &equal_value)
 * it's like sql statement "select * from src_table where name <> equal_value"
 * @param	src_table[IN]: source table
 * @param	result[OUT]: target table
 * @param	name[IN]: field name
 * @param	equal_value[IN]: search string
 * @return	found line count
 * @see
 * @note	data in result is the reference of the src_table
 */
int search_not_equal(const SSTable &src_table, SSTable &result, const string &name, const string &equal_value);

/**
 * int search_is_null(const SSTable &src_table, SSTable &result, const string &name)
 * it's like sql statement "select * from src_table where name is NULL"
 * @param	src_table[IN]: source table
 * @param	result[OUT]: target table
 * @param	name[IN]: field name
 * @return	found line count
 * @see
 * @note	data in result is the reference of the src_table
 */
int search_is_null(const SSTable &src_table, SSTable &result, const string &name);

/**
 * int search_is_not_null(const SSTable &src_table, SSTable &result, const string &name)
 * it's like sql statement "select * from src_table where name is NOT NULL"
 * @param	src_table[IN]: source table
 * @param	result[OUT]: target table
 * @param	name[IN]: field name
 * @return	found line count
 * @see
 * @note	data in result is the reference of the src_table
 */
int search_is_not_null(const SSTable &src_table, SSTable &result, const string &name);

/**
 * int search_less(const SSTable &src_table, SSTable &result, const string &name, const string &value)
 * it's like sql statement "select * from src_table where name < value"
 * @param	src_table[IN]: source table
 * @param	result[OUT]: target table
 * @param	name[IN]: field name
 * @param	value[IN]: compare value
 * @return	found line count
 * @see
 * @note	data in result is the reference of the src_table
 */
int search_less(const SSTable &src_table, SSTable &result, const string &name, const string &value);

/**
 * int search_less_equal(const SSTable &src_table, SSTable &result, const string &name, const string &value)
 * it's like sql statement "select * from src_table where name <= value"
 * @param	src_table[IN]: source table
 * @param	result[OUT]: target table
 * @param	name[IN]: field name
 * @param	value[IN]: compare value
 * @return	found line count
 * @see
 * @note	data in result is the reference of the src_table
 */
int search_less_equal(const SSTable &src_table, SSTable &result, const string &name, const string &value);

/**
 * int search_greater(const SSTable &src_table, SSTable &result, const string &name, const string &value)
 * it's like sql statement "select * from src_table where name > value"
 * @param	src_table[IN]: source table
 * @param	result[OUT]: target table
 * @param	name[IN]: field name
 * @param	value[IN]: compare value
 * @return	found line count
 * @see
 * @note	data in result is the reference of the src_table
 */
int search_greater(const SSTable &src_table, SSTable &result, const string &name, const string &value);

/**
 * int search_greater_equal(const SSTable &src_table, SSTable &result, const string &name, const string &value)
 * it's like sql statement "select * from src_table where name >= value"
 * @param	src_table[IN]: source table
 * @param	result[OUT]: target table
 * @param	name[IN]: field name
 * @param	value[IN]: compare value
 * @return	found line count
 * @see
 * @note	data in result is the reference of the src_table
 */
int search_greater_equal(const SSTable &src_table, SSTable &result, const string &name, const string &value);

/**
 * int table_union(const SSTable &input_a, const SSTable &input_b, SSTable &output)
 * Union input_a and input_b into output
 * Date of input_a and input_b must be the reference from the same table(same buffer),
 * otherwise it will be treated as different row.
 * @param	input_a[IN]: set A
 * @param	input_b[IN]: set B
 * @param	output[OUT]: union set
 * @return	found line count
 * @see
 * @note	data in output is the reference of the original table
 */
int table_union(const SSTable &input_a, const SSTable &input_b, SSTable &output);

/**
 * int table_intersection(const SSTable &input_a, const SSTable &input_b, SSTable &output)
 * Intersection of input_a and input_b into output
 * Date of input_a and input_b must be the reference from the same table(same buffer),
 * otherwise it will be treated as different row.
 * @param	input_a[IN]: set A
 * @param	input_b[IN]: set B
 * @param	output[OUT]: intersection set
 * @return	found line count
 * @see
 * @note	data in output is the reference of the original table
 */
int table_intersection(const SSTable &input_a, const SSTable &input_b, SSTable &output);


//for developer
typedef bool (ss::SSTable::row_type::*func_comp)(const string&, const ss::SSTable::row_type&) const;
int search_by_compare_function(const SSTable &src_table, SSTable &result, const string &name, const string &equal_value, func_comp comp);

//copy table head, dst table will be cleared.
void copy_table_head(ss::SSTable &dst, const ss::SSTable &src);

}

#endif
