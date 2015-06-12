#include "SSFunctional.h"
#include "SSTable.h"
#include "SSHead.h"

namespace ss
{

void create_table(const SSHead &head, SSTable &table)
{
    table.clear();
    table.result().set_header(head.get_header());
}

void print_table(ostream &of, const SSTable &table)
{
    for(int i = 0; i < table.get_field_count(); i++){
        of << table.get_field_name(i) << "\t";
    }
    of << std::endl;
    for(SSTable::const_iterator it = table.begin(); it != table.end(); ++it){
        for(int i = 0; i < table.get_field_count(); i++){
            of << it->get<string>(table.get_field_name(i)) << "\t";
        }
        of << std::endl;
    }
    of.flush();
}

}
