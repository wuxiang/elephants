#if defined(MYSQL_DRIVER)

#include "SSSQLCreator_Mysql.h"
#include "SSMyLog.h"

#define SQLFIELD(x) ("`"+x+"`")
#define SQLSTR(x) ("\'" + x + "\'")

using std::string;

SSSQLCreator_Mysql::SSSQLCreator_Mysql()
{

}

SSSQLCreator_Mysql::~SSSQLCreator_Mysql()
{

}

bool SSSQLCreator_Mysql::ParseModify(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	for (lstRow::const_iterator iter = lstIn.begin(); iter != lstIn.end(); ++iter)
	{
		ss::SSTable::row_type* pRow = (*iter);
		string sqlstr = "UPDATE " + m_sTableName + " SET ";
		for (int i = 0; i < pTable->result().get_header().field_count(); ++i)
		{
			string name = pTable->result().get_header().fields(i).name;
			if(false == pRow->is_modified(name))
			{
				continue;
			}

			sqlstr += FormatColName(name);
			sqlstr += " = ";
			if(!pRow->assigned(name))
			{
				sqlstr += "NULL";
			}

			string sfmtval = FormatVal(pTable->result().get_header().fields(i).type, pRow->get<string>(name));
			sqlstr += sfmtval;
			sqlstr += ",";
		}
		//remove the last ,
		sqlstr.erase(sqlstr.end() - 1);

		//where statement
		string where = " WHERE";
		for(int i = 0; i < pTable->result().get_header().primary_count(); ++i){
			if(i){
				where += " AND ";
			}
			string name = pTable->result().get_header().get_primary(i);
			where += (" " + FormatColName(name) + " = " + FormatVal(pTable->result().get_header().fields(i).type, pRow->get<string>(name)));
		}
		sqlstr += where;

		lstRet.push_back(sqlstr);
	}
	return true;
}

bool SSSQLCreator_Mysql::ParseDelete(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	for (lstRow::const_iterator iter = lstIn.begin(); iter != lstIn.end(); ++iter)
	{
		ss::SSTable::row_type* pRow = (*iter);

		string sqlstr = "DELETE FROM " + m_sTableName + " WHERE ";
		for(int i = 0; i < pTable->result().get_header().primary_count(); ++i)
		{
			if(i){
				sqlstr += " AND ";
			}
			string name = pTable->result().get_header().get_primary(i);
			sqlstr += (FormatColName(name) + " = " + FormatVal(pTable->result().get_header().fields(name).type, pRow->get<string>(name)));
		}

		lstRet.push_back(sqlstr);
	}
	return true;
}
/*
//批量插入版本
bool SSSQLCreator_Mysql::ParseInsert_mode0(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	string sInsertHead = "INSERT INTO " + m_sTableName + "(";

	std::vector<string> vctColName;
	for (int i = 0; i < pTable->result().get_header().field_count(); ++i)
	{
		string sColName = pTable->result().get_header().fields(i).name;
		vctColName.push_back(sColName);

		sInsertHead += FormatColName(sColName);
		sInsertHead += ",";
	}
	sInsertHead[sInsertHead.size() - 1] = ')';
	sInsertHead += " VALUES ";

	//批量插入,提升效率
	int nBatCnt = 100;
	int nCurCnt = 0;
	string sFmtVal;
	string sExecute = sInsertHead;
	for (lstRow::const_iterator iterR = lstIn.begin(); iterR != lstIn.end(); ++iterR)
	{
		nCurCnt++;
		ss::SSTable::row_type* pRow = (*iterR);
		string sContent = "(";
		for (std::vector<string>::iterator iterF = vctColName.begin(); iterF != vctColName.end(); ++iterF)
		{
			const ss::field *pF = pRow->get_field_by_name(*iterF);
			if (!pF)
			{
				sFmtVal = "NULL";
			}
			if (!pF->assigned())
			{
				sFmtVal = "NULL";
			}
			else
			{
				sFmtVal = FormatVal(pF->type(), pRow->get<string>(*iterF));
			}

			sContent += sFmtVal;
			sContent += ",";
		}
		sContent[sContent.size() - 1] = ')';

		sExecute += sContent;
		sExecute += ",";

		if (nCurCnt == nBatCnt)
		{
			nCurCnt = 0;
			sExecute.erase(sExecute.end() - 1);
			lstRet.push_back(sExecute);
			sExecute = sInsertHead;
			if (lstRet.size() == lstIn.size())
			{
				return true;
			}
		}
	}
	sExecute.erase(sExecute.end() - 1);
	lstRet.push_back(sExecute);
	return true;
}
*/


//原先版本
bool SSSQLCreator_Mysql::ParseInsert_mode0(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	for (lstRow::const_iterator iter = lstIn.begin(); iter != lstIn.end(); ++iter)
	{
		ss::SSTable::row_type* pRow = (*iter);

		string sqlstr = "INSERT INTO " + m_sTableName + "(";
 		string values = "VALUES (";

		for(int i = 0; i < pTable->result().get_header().field_count(); ++i)
		{
			string name = pTable->result().get_header().fields(i).name;
			const ss::field *pF = pRow->get_field_by_name(name);
			if (!pF)
			{
				continue;
			}
			if(!pF->assigned())
			{
				continue;
			}

			sqlstr += FormatColName(name);
			sqlstr += ",";

			string sfmtval = FormatVal(pF->type(), pRow->get<string>(name));
			values += sfmtval;

			if(pRow->get<string>(name).empty())
			{
				DA_ERRLOG("%s: value.get<string>(%s).empty():[%s,%s]", __FUNCTION__, name.c_str(), sqlstr.c_str(), values.c_str());
			}

			values += ",";

		}
		//replace the last , as )
		sqlstr[sqlstr.size() - 1] = ')';
		values[values.size() - 1] = ')';
		sqlstr += " ";
		sqlstr += values;

		lstRet.push_back(sqlstr);
	}
	return true;
}


bool SSSQLCreator_Mysql::ParseInsert_mode1(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	for (lstRow::const_iterator iter = lstIn.begin(); iter != lstIn.end(); ++iter)
	{
		ss::SSTable::row_type* pRow = (*iter);

		string sqlstr = "REPLACE INTO " + m_sTableName + "(";
		string values = "VALUES (";

		for(int i = 0; i < pTable->result().get_header().field_count(); ++i)
		{
			string name = pTable->result().get_header().fields(i).name;
			const ss::field *pF = pRow->get_field_by_name(name);
			if (!pF)
			{
				continue;
			}
			if(!pF->assigned())
			{
				continue;
			}

			sqlstr += FormatColName(name);
			sqlstr += ",";

			string sfmtval = FormatVal(pF->type(), pRow->get<string>(name));
			values += sfmtval;

			if(pRow->get<string>(name).empty())
			{
				DA_ERRLOG("%s: value.get<string>(%s).empty():[%s,%s]", __FUNCTION__, name.c_str(), sqlstr.c_str(), values.c_str());
			}

			values += ",";

		}
		//replace the last , as )
		sqlstr[sqlstr.size() - 1] = ')';
		values[values.size() - 1] = ')';
		sqlstr += " ";
		sqlstr += values;

		lstRet.push_back(sqlstr);
	}
	return true;
}

bool SSSQLCreator_Mysql::ParseInsert_mode2(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	for (lstRow::const_iterator iter = lstIn.begin(); iter != lstIn.end(); ++iter)
	{
		ss::SSTable::row_type* pRow = (*iter);

		string sqlstr = "INSERT INTO " + m_sTableName + "(";
		string values = "VALUES (";
		string on_duplcate_key_update;

		for(int i = 0; i < pTable->result().get_header().field_count(); ++i)
		{
			string name = pTable->result().get_header().fields(i).name;
			const ss::field *pF = pRow->get_field_by_name(name);
			if (!pF)
			{
				continue;
			}
			if(!pF->assigned())
			{
				continue;
			}

			sqlstr += FormatColName(name);
			sqlstr += ",";

			string sfmtval = FormatVal(pF->type(), pRow->get<string>(name));
			values += sfmtval;

			if(pRow->get<string>(name).empty())
			{
				DA_ERRLOG("%s: value.get<string>(%s).empty():[%s,%s]", __FUNCTION__, name.c_str(), sqlstr.c_str(), values.c_str());
			}

			values += ",";

			on_duplcate_key_update += FormatColName(name);
			on_duplcate_key_update += " = ";

			sfmtval = FormatVal(pTable->result().get_header().fields(i).type, pRow->get<string>(name));
			on_duplcate_key_update += sfmtval;

			on_duplcate_key_update += ",";
		}
		//replace the last , as )
		sqlstr[sqlstr.size() - 1] = ')';
		values[values.size() - 1] = ')';
		sqlstr += " ";
		sqlstr += values;
		
		on_duplcate_key_update[on_duplcate_key_update.size() - 1] = ' ';
		sqlstr += " ON DUPLICATE KEY UPDATE ";
		sqlstr += on_duplcate_key_update;

		lstRet.push_back(sqlstr);
	}
	return true;
}

std::string SSSQLCreator_Mysql::FormatColName(const std::string& sColName)
{
	return SQLFIELD(sColName);
}

std::string SSSQLCreator_Mysql::FormatVal(ss::uint8 valtype, const std::string sVal)
{
	switch (valtype)
	{
	case ss::SSTYPE_INT8:
	case ss::SSTYPE_UINT8:
	case ss::SSTYPE_INT16:
	case ss::SSTYPE_UINT16:
	case ss::SSTYPE_INT32:
	case ss::SSTYPE_UINT32:
	case ss::SSTYPE_INT64:
	case ss::SSTYPE_UINT64:
	case ss::SSTYPE_BIT64:
		return sVal;
	default:
		return SQLSTR(EscapeStr(sVal));
	}
}
#endif
