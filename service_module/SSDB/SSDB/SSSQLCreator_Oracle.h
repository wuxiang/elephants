#ifndef __SS_SQLCREATOR_ORACLE_H
#define __SS_SQLCREATOR_ORACLE_H

#include "SSSQLCreator.h"

class SSSQLCreator_Oracle : public SSSQLCreator
{
public:
	SSSQLCreator_Oracle();
	virtual ~SSSQLCreator_Oracle();

protected:
	virtual bool ParseModify(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseDelete(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseInsert_mode0(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseInsert_mode1(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual bool ParseInsert_mode2(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet);
	virtual std::string FormatColName(const std::string& sColName);
	virtual std::string FormatVal(ss::uint8 valtype, const std::string sVal);

	std::string BuildClause_Merge();
	bool BuildClause_Using(const lstRow& lstIn, const std::vector<std::string>& vctColName, int nBatCnt, std::list<std::string>& lstRet);
	std::string BuildClause_On(const std::vector<std::string>& vctPK);
	std::string BuildClause_Match_Update(const std::vector<std::string>& vctNotPKCol);
	std::string BuildClause_Match_Delete(const std::vector<std::string>& vctNotPKCol);
	std::string BuildClause_NotMatch(const std::vector<std::string>& vctColName);
	//m_sTableName前后加双引号
	std::string GetCompatibleTableName();

	bool NeedUseParam(const std::string& sVal);
};


#endif /** __SS_SQLCREATOR_ORACLE_H */