#ifndef __SS_HEAD_H
#define __SS_HEAD_H 1
#include "SSTuple.h"
#include <string>

using std::string;
namespace ss {

class SSHead
{
public:
    void add_string(const string &name);
    void add_double(const string &name, int decimals);
    void add_bit(const string &name, int length = 64);
    void add_datetime(const string &name);
    void add_date(const string &name);
    void add_int32(const string &name);
    void clear();
    const tuple::header &get_header() const {return m_head;}
private:
    tuple::header	m_head;
};

}//namespace

#endif
