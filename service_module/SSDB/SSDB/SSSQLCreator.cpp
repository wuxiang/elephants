#include "SSSQLCreator.h"
#include "SSSQLCreator_Mysql.h"
#if defined(OCI_DRIVER)
#include "SSSQLCreator_Oracle.h"
#endif
using namespace ss;

SSSQLCreator::SSSQLCreator()
:m_sTableName("")
,m_pDriver(NULL)
{

}

SSSQLCreator::~SSSQLCreator()
{

}

boost::shared_ptr<SSSQLCreator> SSSQLCreator::CreateByType(const std::string& sType)
{
    #if defined(MYSQL_DRIVER)
	if (sType == "mysql")
	{
		return boost::shared_ptr<SSSQLCreator>(new SSSQLCreator_Mysql());
	}
	#endif
	
	#if defined(OCI_DRIVER)
	if (sType == "oci")
	{
		return boost::shared_ptr<SSSQLCreator>(new SSSQLCreator_Oracle());
	}
	#endif

	else
	{
		return boost::shared_ptr<SSSQLCreator>();
	}
}

void SSSQLCreator::SetTableName(const std::string& sTableName)
{
	m_sTableName = sTableName;
}

void SSSQLCreator::SetDBDriver(ss::db_driver* pDriver)
{
	m_pDriver = pDriver;
}

typedef std::set<block *> SetBlk;

bool SSSQLCreator::ParseDelete(std::set<ss::block *>& InputData, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	if (InputData.empty())
	{
		return true;
	}
	lstRow DeleteRows;
	for (SetBlk::iterator iter = InputData.begin(); iter != InputData.end(); ++iter)
	{
		SSTable::row_type *pRow = new SSTable::row_type(pTable, db_row(*iter));
		if (pRow->is_deleted())
		{
			DeleteRows.push_back(pRow);
		}
		else
		{
			delete pRow;
		}
	}

	bool bRet = ParseDelete(DeleteRows, pTable, lstRet);
	ss::ClearContainer(DeleteRows);
	return bRet;
}

bool SSSQLCreator::ParseInsertAndModify(std::map<int, ss::block *>& InputData, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	if (InputData.empty())
	{
		return true;
	}
	bool bRet = true;
	lstRow InsertRows;
	lstRow ModifyRows;
	for (std::map<int, ss::block *>::iterator iter = InputData.begin(); iter != InputData.end(); ++iter)
	{
		SSTable::row_type *pRow = new SSTable::row_type(pTable, db_row(iter->second));
		if (pRow->is_inserted())
		{
			InsertRows.push_back(pRow);
		}
		else if (pRow->is_modified())
		{
			ModifyRows.push_back(pRow);
		}
		else
		{
			delete pRow;
		}
	}

	int nInsertMode = pTable->get_insert_mode();
	if (0 == nInsertMode)
	{
		bRet &= ParseInsert_mode0(InsertRows, pTable, lstRet);
	}
	else if (1 == nInsertMode)
	{
		bRet &= ParseInsert_mode1(InsertRows, pTable, lstRet);
	}
	else if (2 == nInsertMode)
	{
		bRet &= ParseInsert_mode2(InsertRows, pTable, lstRet);
	}

	bRet &= ParseModify(ModifyRows, pTable, lstRet);

	ss::ClearContainer(InsertRows);
	ss::ClearContainer(ModifyRows);
	return bRet;
}

bool SSSQLCreator::ParseModify(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	return false;
}

bool SSSQLCreator::ParseDelete(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	return false;
}

bool SSSQLCreator::ParseInsert_mode0(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	return false;
}

bool SSSQLCreator::ParseInsert_mode1(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	return false;
}

bool SSSQLCreator::ParseInsert_mode2(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	return false;
}

std::string SSSQLCreator::FormatColName(const std::string& sColName)
{
	return sColName;
}

std::string SSSQLCreator::FormatVal(ss::uint8 valtype, const std::string sVal)
{
	return sVal;
}

std::string SSSQLCreator::EscapeStr(const std::string& sInput)
{
	std::string sRet = sInput;
	if (m_pDriver)
	{
		m_pDriver->real_escape_string(sRet);
	}
	return sRet;
}