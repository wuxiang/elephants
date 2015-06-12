#if defined(OCI_DRIVER)

#include "SSSQLCreator_Oracle.h"
#include "SSMyLog.h"

using std::string;

SSSQLCreator_Oracle::SSSQLCreator_Oracle()
{

}

SSSQLCreator_Oracle::~SSSQLCreator_Oracle()
{

}

bool SSSQLCreator_Oracle::ParseModify(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	for (lstRow::const_iterator iter = lstIn.begin(); iter != lstIn.end(); ++iter)
	{
		ss::SSTable::row_type* pRow = (*iter);
		string sqlstr = "UPDATE " + GetCompatibleTableName() + " SET ";
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
			else
			{
				string sfmtval = FormatVal(pTable->result().get_header().fields(i).type, pRow->get<string>(name));
				sqlstr += sfmtval;
			}

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

bool SSSQLCreator_Oracle::ParseDelete(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	for (lstRow::const_iterator iter = lstIn.begin(); iter != lstIn.end(); ++iter)
	{
		ss::SSTable::row_type* pRow = (*iter);

		string sqlstr = "DELETE FROM " + GetCompatibleTableName() + " WHERE ";
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
bool SSSQLCreator_Oracle::ParseInsert_mode0(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	string sInsertHead = "INSERT INTO " + GetCompatibleTableName() + "(";

	std::vector<string> vctColName;
	for (int i = 0; i < pTable->result().get_header().field_count(); ++i)
	{
		string sColName = pTable->result().get_header().fields(i).name;
		vctColName.push_back(sColName);

		sInsertHead += FormatColName(sColName);
		sInsertHead += ",";
	}
	sInsertHead[sInsertHead.size() - 1] = ')';
	sInsertHead += "";

	//批量插入,提升效率
	int nBatCnt = 100;
	int nCurCnt = 0;
	string sFmtVal;
	string sExecute = sInsertHead;
	for (lstRow::const_iterator iterR = lstIn.begin(); iterR != lstIn.end(); ++iterR)
	{
		nCurCnt++;
		ss::SSTable::row_type* pRow = (*iterR);
		string sContent = " SELECT ";
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
		sContent.erase(sContent.end() - 1);

		sExecute += sContent;
		sExecute += " FROM DUAL ";
		sExecute += "UNION";
		
		if (nCurCnt == nBatCnt)
		{
			nCurCnt = 0;
			sExecute.erase(sExecute.end() - 5, sExecute.end());
			lstRet.push_back(sExecute);
			sExecute = sInsertHead;
			if (lstRet.size() == lstIn.size())
			{
				return true;
			}
		}
	}
	sExecute.erase(sExecute.end() - 5, sExecute.end());
	lstRet.push_back(sExecute);
	return true;
}*/

//原先版本
bool SSSQLCreator_Oracle::ParseInsert_mode0(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	std::list<std::string> lstAppend;
	for (lstRow::const_iterator iter = lstIn.begin(); iter != lstIn.end(); ++iter)
	{
		ss::SSTable::row_type* pRow = (*iter);

		lstAppend.clear();

		string sqlstr = "INSERT INTO " + GetCompatibleTableName() + "(";
 		string values = "VALUES (";

		string declareclause = "DECLARE ";
		bool bUseParam = false;

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

			string sVal = pRow->get<string>(name);
			string sfmtval = FormatVal(pF->type(), sVal);
			if (NeedUseParam(sfmtval))
			{
				bUseParam = true;
				values += name;
				declareclause += name;
				declareclause += " clob := ";
				declareclause += sfmtval;
				declareclause += ";";
			}
			else
			{
				values += sfmtval;
			}


			values += ",";

		}
		//replace the last , as )
		sqlstr[sqlstr.size() - 1] = ')';
		values[values.size() - 1] = ')';
		sqlstr += " ";
		sqlstr += values;

		if (bUseParam)
		{
			sqlstr = "BEGIN " + sqlstr + ";" + "END;";
			sqlstr = declareclause + sqlstr;

		}

		lstRet.push_back(sqlstr);
	}
	return true;
}

bool SSSQLCreator_Oracle::ParseInsert_mode1(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	std::vector<string> vctPK;
	for (int i = 0; i < pTable->result().get_header().primary_count(); ++i)
	{
		string sPK = pTable->result().get_header().get_primary(i);
		vctPK.push_back(sPK);
	}

	std::vector<string> vctColName;
	std::vector<string> vctNotPKCol;
	for (int i = 0; i < pTable->result().get_header().field_count(); ++i)
	{
		string sColName = pTable->result().get_header().fields(i).name;
		vctColName.push_back(sColName);

		bool bIsPK = false;
		for (size_t j = 0; j < vctPK.size(); ++j)
		{
			if (sColName == vctPK[j])
			{
				bIsPK = true;
				break;
			}
		}
		if (!bIsPK)
		{
			vctNotPKCol.push_back(sColName);
		}
	}

	std::list<string> lstClause;
	if (!BuildClause_Using(lstIn, vctColName, 1, lstClause))
	{
		return false;
	}

	string sClause_Merge = BuildClause_Merge();
	string sClause_On = BuildClause_On(vctPK);
	string sClause_Match = BuildClause_Match_Delete(vctNotPKCol);
	string sClause_NotMatch = BuildClause_NotMatch(vctColName);

	string sExecute;
	//先删除
	for (std::list<string>::iterator iter = lstClause.begin(); iter != lstClause.end(); ++iter)
	{
		sExecute = sClause_Merge;
		sExecute += *iter;
		sExecute += sClause_On;
		sExecute += sClause_Match;

		lstRet.push_back(sExecute);
	}
	//再添加
	for (std::list<string>::iterator iter = lstClause.begin(); iter != lstClause.end(); ++iter)
	{
		sExecute = sClause_Merge;
		sExecute += *iter;
		sExecute += sClause_On;
		sExecute += sClause_NotMatch;

		lstRet.push_back(sExecute);
	}

	return true;
}

bool SSSQLCreator_Oracle::ParseInsert_mode2(const lstRow& lstIn, ss::SSTable* pTable, std::list<std::string>& lstRet)
{
	std::vector<string> vctPK;
	for (int i = 0; i < pTable->result().get_header().primary_count(); ++i)
	{
		string sPK = pTable->result().get_header().get_primary(i);
		vctPK.push_back(sPK);
	}

	std::vector<string> vctColName;
	std::vector<string> vctNotPKCol;
	for (int i = 0; i < pTable->result().get_header().field_count(); ++i)
	{
		string sColName = pTable->result().get_header().fields(i).name;
		vctColName.push_back(sColName);

		bool bIsPK = false;
		for (size_t j = 0; j < vctPK.size(); ++j)
		{
			if (sColName == vctPK[j])
			{
				bIsPK = true;
				break;
			}
		}
		if (!bIsPK)
		{
			vctNotPKCol.push_back(sColName);
		}
	}

	std::list<string> lstClause;
	if (!BuildClause_Using(lstIn, vctColName, 100, lstClause))
	{
		return false;
	}

	string sClause_Merge = BuildClause_Merge();
	string sClause_On = BuildClause_On(vctPK);
	string sClause_Match = BuildClause_Match_Update(vctNotPKCol);
	string sClause_NotMatch = BuildClause_NotMatch(vctColName);

	string sExecute;
	for (std::list<string>::iterator iter = lstClause.begin(); iter != lstClause.end(); ++iter)
	{
		sExecute = sClause_Merge;
		sExecute += *iter;
		sExecute += sClause_On;
		sExecute += sClause_Match;
		sExecute += sClause_NotMatch;

		lstRet.push_back(sExecute);
	}

	return true;
}

std::string SSSQLCreator_Oracle::FormatColName(const std::string& sColName)
{
	std::string sRet = "\"" + sColName + "\"";
	return sRet;
}

std::string SSSQLCreator_Oracle::FormatVal(ss::uint8 valtype, const std::string sVal)
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
	case ss::SSTYPE_T32:
	case ss::SSTYPE_T64:
		return "to_date('" + sVal + "','yyyy-MM-DD hh24:mi:ss')";
	default:
		return "'" + EscapeStr(sVal) + "'";
	}
}

std::string SSSQLCreator_Oracle::BuildClause_Merge()
{
	return "MERGE INTO " + GetCompatibleTableName() + " t1";
}

bool SSSQLCreator_Oracle::BuildClause_Using(const lstRow& lstIn, const std::vector<std::string>& vctColName,int nBatCnt, std::list<std::string>& lstRet)
{
	string sHead = " USING (";
	string sTail = ") t2";
	//批量插入,提升效率
	//int nBatCnt = 100;
	int nCurCnt = 0;
	string sFmtVal;
	string sExecute = sHead;
	for (lstRow::const_iterator iterR = lstIn.begin(); iterR != lstIn.end(); ++iterR)
	{
		nCurCnt++;
		ss::SSTable::row_type* pRow = (*iterR);
		string sContent = " SELECT ";
		for (std::vector<string>::const_iterator iterF = vctColName.begin(); iterF != vctColName.end(); ++iterF)
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
			sContent += " as ";
			sContent += FormatColName(*iterF);
			sContent += ",";
		}
		sContent.erase(sContent.end() - 1);

		sExecute += sContent;
		sExecute += " FROM DUAL ";
		sExecute += "UNION";

		if (nCurCnt == nBatCnt)
		{
			nCurCnt = 0;
			sExecute.erase(sExecute.end() - 5, sExecute.end());
			sExecute += sTail;
			lstRet.push_back(sExecute);
			sExecute = sHead;
			if (lstRet.size() == lstIn.size())
			{
				return true;
			}
		}
	}
	sExecute.erase(sExecute.end() - 5, sExecute.end());
	sExecute += sTail;
	lstRet.push_back(sExecute);
	return true;
}

std::string SSSQLCreator_Oracle::BuildClause_On(const std::vector<std::string>& vctPK)
{
	string sRet = " ON (";
	for (std::vector<string>::const_iterator iter = vctPK.begin(); iter != vctPK.end(); ++iter)
	{
		sRet += "t1.";
		sRet += FormatColName(*iter);
		sRet += " = ";
		sRet += "t2.";
		sRet += FormatColName(*iter);
		sRet += " and";
	}
	sRet.erase(sRet.end() - 4, sRet.end());
	sRet += ")";
	return sRet;
}

std::string SSSQLCreator_Oracle::BuildClause_Match_Update(const std::vector<std::string>& vctNotPKCol)
{
	string sRet = " WHEN MATCHED THEN UPDATE SET ";
	for (std::vector<string>::const_iterator iter = vctNotPKCol.begin(); iter != vctNotPKCol.end(); ++iter)
	{
		sRet += "t1.";
		sRet += FormatColName(*iter);
		sRet += " = ";
		sRet += "t2.";
		sRet += FormatColName(*iter);
		sRet += ",";
	}
	sRet.erase(sRet.end() - 1);
	return sRet;
}

std::string SSSQLCreator_Oracle::BuildClause_Match_Delete(const std::vector<std::string>& vctNotPKCol)
{
	string sRet = " WHEN MATCHED THEN UPDATE SET ";
	for (std::vector<string>::const_iterator iter = vctNotPKCol.begin(); iter != vctNotPKCol.end(); ++iter)
	{
		sRet += "t1.";
		sRet += FormatColName(*iter);
		sRet += " = ";
		sRet += "t2.";
		sRet += FormatColName(*iter);
		sRet += ",";
	}
	sRet.erase(sRet.end() - 1);
	sRet += " DELETE WHERE 1 = 1";
	return sRet;
}

std::string SSSQLCreator_Oracle::BuildClause_NotMatch(const std::vector<std::string>& vctColName)
{
	string sDest = "(";
	string sSrc = "(";
	for (std::vector<string>::const_iterator iter = vctColName.begin(); iter != vctColName.end(); ++iter)
	{
		sDest += "t1.";
		sDest += FormatColName(*iter);
		sDest += ",";
		sSrc += "t2.";
		sSrc += FormatColName(*iter);
		sSrc += ",";
	}
	sDest[sDest.size() - 1] = ')';
	sSrc[sSrc.size() - 1] = ')';

	string sRet = " WHEN NOT MATCHED THEN INSERT ";
	sRet += sDest;
	sRet += " VALUES ";
	sRet += sSrc;
	return sRet;
}

std::string SSSQLCreator_Oracle::GetCompatibleTableName()
{
	return "\"" + m_sTableName + "\"";
}

bool SSSQLCreator_Oracle::NeedUseParam(const std::string& sVal)
{
	if (sVal.size() > 3800)
	{
		return true;
	}
	return false;
}

#endif