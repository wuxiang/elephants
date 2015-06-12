#ifndef __SS_FUNCTIONAL_H
#define __SS_FUNCTIONAL_H 1
#include <ostream>
namespace ss {

class SSTable;
class SSHead;
using std::ostream;

void create_table(const SSHead &head, SSTable &table);
void print_table(ostream &of, const SSTable &table);

}//namespace

#endif
