#ifndef __SS_SQLCREATOR_H
#define __SS_SQLCREATOR_H

#include "SSClearContainer.h"
#include "SSTable.h"
#include "boost/shared_ptr.hpp"


typedef std::list<ss::SSTable::row_type*> lstRow;

class SSSQLCreator
{
protected:
	SSSQLCreator();
public:
	virtual ~SSSQLCreator();

	static boost::shared_ptr<SSSQLCreator> CreateByType(const std::string& sType);
	void SetTableName(const std::string& sTableName);
	void SetDBDriver(ss::db_driver* pDriver);
	bool ParseDelete(std::set<ss::block *>& InputData, ss::SSTable* pTable, std::list<std::string>& lstRet);
	bool ParseInsertAndModify(std::map<int, ss::block *>& InputData, ss::SSTable* pTable, std::list<std::string>& lstRet);

protected:
	virtual bool ParseModify(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseDelete(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseInsert_mode0(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseInsert_mode1(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseInsert_mode2(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual std::string FormatColName(const std::string& sColName);
	virtual std::string FormatVal(ss::uint8 valtype, const std::string sVal);
	virtual std::string EscapeStr(const std::string& sInput);

protected:
	std::string m_sTableName;
	ss::db_driver* m_pDriver;
};


#endif /** __SS_SQLCREATOR_H */
