#include "SSHead.h"

namespace ss
{
void
SSHead::add_string(const string &name)
{
    field_def field;
    field.name = name;
    field.type = SSTYPE_STRING;
    m_head.add(field);
}

void
SSHead::add_double(const string &name, int decimals)
{
    field_def field;
    field.name = name;
    field.type = SSTYPE_DOUBLE;
    field.decimals = decimals;
    m_head.add(field);
}

void
SSHead::add_bit(const string &name, int length)
{
    field_def field;
    field.name = name;
    field.type = SSTYPE_BIT64;
    field.length = length;
    m_head.add(field);
}

void
SSHead::add_datetime(const string &name)
{
    field_def field;
    field.name = name;
    field.type = SSTYPE_T32;
    field.length = 19; //0000-00-00 00:00:00
    m_head.add(field);
}

void
SSHead::add_date(const string &name)
{
    field_def field;
    field.name = name;
    field.type = SSTYPE_T32;
    field.length = 10; //date(YYYY-MM-DD)
    m_head.add(field);
}

void
SSHead::add_int32(const string &name)
{
    field_def field;
    field.name = name;
    field.type = SSTYPE_INT32;
    m_head.add(field);
}

void
SSHead::clear()
{
    m_head.clear();
}

}
