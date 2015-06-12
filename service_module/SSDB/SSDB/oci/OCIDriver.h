#ifndef __OCI_DRIVER_H
#define __OCI_DRIVER_H

#include "oci_Data.h"
#include "oci_HandleMng.h"
#include <vector>

class oci_dirver : public ss::db_driver
{
public:
	oci_dirver();
	virtual ~oci_dirver();

public:
	void split_param(const std::string& param) ;
	bool open(const std::string& param);
	void close();
	bool execute(const char * sqlstr);
	bool query(ss::db_result& result , const char * sqlstr , int limmited = -1);
	bool begin() ;
	bool rollback() ;
	bool commit() ;

	std::string &real_escape_string(std::string &from) const;

protected:
	void ParseOptionalParam(const std::string& sOptionParam);
	ub4 GetConnMode();
	bool Check(sword status);
	bool InitPrimaryKey();
	bool Prepare(const char * sqlstr);
	bool InitHeader(ss::db_result::header& refHead);
	bool AddRow(ss::db_result& result);

private:
	oci_HandleMng m_HandleMng;
	oci_RowDataBuf m_RowBuff;
	std::vector<std::string> m_vctPK;

	std::string tablename_;
	std::string rolename_;
};


#endif /** __OCI_DRIVER_H */
