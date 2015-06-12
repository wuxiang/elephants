#ifndef __SS_SQLCREATOR_MYSQL_H
#define __SS_SQLCREATOR_MYSQL_H

#include "SSSQLCreator.h"

class SSSQLCreator_Mysql : public SSSQLCreator
{
public:
	SSSQLCreator_Mysql();
	virtual ~SSSQLCreator_Mysql();

protected:
	virtual bool ParseModify(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseDelete(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseInsert_mode0(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseInsert_mode1(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseInsert_mode2(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual std::string FormatColName(const std::string& sColName);
	virtual std::string FormatVal(ss::uint8 valtype, const std::string sVal);
};

#endif /** __SS_SQLCREATOR_MYSQL_H */