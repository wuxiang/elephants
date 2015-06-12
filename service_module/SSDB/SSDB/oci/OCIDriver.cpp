#if defined(OCI_DRIVER)

#include "OCIDriver.h"
#include "SSMyLog.h"
#include <boost/algorithm/string.hpp>

#define COL_DATA_BUF_SIZE 32

int replace_all(std::string& str,  const std::string& pattern,  const std::string& newpat) 
{ 
	int count = 0; 
	const size_t nsize = newpat.size(); 
	const size_t psize = pattern.size(); 

	for(size_t pos = str.find(pattern, 0);  
		pos != std::string::npos; 
		pos = str.find(pattern,pos + nsize)) 
	{ 
		str.replace(pos, psize, newpat); 
		count++; 
	} 

	return count; 
}

using namespace ss;

/////////////////////////class oci_dirver/////////////////////////
oci_dirver::oci_dirver()
:db_driver("oci")
{

}

oci_dirver::~oci_dirver()
{
	close();
}

void oci_dirver::split_param(const std::string& param)
{
	const char * str = param.c_str() ;
	int count = 0 , slen = 0;
	char strbuf[256] = {'\0'} ;
	char c = 0 ;

	do{
		c = *str ;
		if(c == '\0' || c == ':')
		{
			strbuf[slen] = '\0' ;
			switch(count)
			{
			case 0 : host_ = strbuf ; break ;
			case 1 : port_ = strbuf ; break ;
			case 2 : username_ = strbuf ; break ;
			case 3 : password_ = strbuf ; break ;
			case 4 : database_ = strbuf ; break ;
			default: ParseOptionalParam(strbuf); break ;
			}		

			count++ ;
			slen = 0 ;
			strbuf[0] = '\0' ;

			if(c == ':')
			{
				str++ ;
			}
		}
		else
		{
			strbuf[slen] = c ;
			slen++ ;
			str++ ;
		}	
	}while(c != '\0') ;	
}

bool oci_dirver::open(const std::string& param)
{
	//"oci:host:port:username:password:database:-t tablename:-r sysdba"
	std::string sParam = param;
	sParam = sParam.substr(4, sParam.length() - 4);
	split_param(sParam.c_str()); 
	std::string sDbLink = host_ + ':' +  port_ + '/' + database_;

	//环境句柄
	sword swRet;
	//对象模式初始化
	ub4 EnvMode = OCI_OBJECT;
	//ub4 EnvMode = OCI_DEFAULT;
	swRet = OCIEnvCreate(&m_HandleMng.hp_env, EnvMode, NULL, 
		(void  * (*)(void  *, size_t))NULL, 
		(void  * (*)(void  *, void  *, size_t))NULL, 
		(void (*)(void  *, void  *))NULL, 
		0, NULL);


	if (!Check(swRet))
	{	
	    DA_ERRLOG( "Oracle environment initialization error!");
		DA_FLUSH(ERROR_FLUSH);
		errmsg_="Oracle environment initialization error!";
		return false;
	}
	DA_FILELOG("Oracle environment initialization success!");

	m_HandleMng.InitAll();

	swRet = OCIServerAttach(m_HandleMng.hp_serv, m_HandleMng.hp_err, (const OraText *)sDbLink.c_str(), sDbLink.length(), 0);
	if (!Check(swRet))
	{
		DA_ERRLOG("Oracle server attach error!");
		DA_FLUSH(ERROR_FLUSH);
		errmsg_="Oracle server attach error!";
		return false;
	}

	DA_FILELOG("Oracle server attach success!");
	swRet = OCIAttrSet((dvoid *)m_HandleMng.hp_ctx, OCI_HTYPE_SVCCTX, (dvoid *)m_HandleMng.hp_serv, (ub4)0, OCI_ATTR_SERVER, m_HandleMng.hp_err);
	
	//用户名、密码
	swRet = OCIAttrSet((dvoid *)m_HandleMng.hp_auth, (ub4)OCI_HTYPE_SESSION , (dvoid *)username_.c_str() , (ub4)strlen(username_.c_str()), (ub4)OCI_ATTR_USERNAME, m_HandleMng.hp_err);
	swRet = OCIAttrSet((dvoid *)m_HandleMng.hp_auth, (ub4)OCI_HTYPE_SESSION, (dvoid *)password_.c_str(), (ub4)strlen(password_.c_str()), (ub4)OCI_ATTR_PASSWORD, m_HandleMng.hp_err);
	//创建会话连接

	swRet = OCISessionBegin(m_HandleMng.hp_ctx, m_HandleMng.hp_err, m_HandleMng.hp_auth, OCI_CRED_RDBMS, GetConnMode());
	if(!Check(swRet))
	{
		DA_ERRLOG("Oracle session begin error!");
		DA_FLUSH(ERROR_FLUSH);
		errmsg_="Oracle session begin error!";
		return false;
	}
	DA_FILELOG("User session success!");

	swRet = OCIAttrSet((dvoid *)m_HandleMng.hp_ctx, (ub4) OCI_HTYPE_SVCCTX, (dvoid *)m_HandleMng.hp_auth, (ub4)0, (ub4)OCI_ATTR_SESSION, m_HandleMng.hp_err);
	
	//execute("alter session set nls_date_language=AMERICAN");
	//execute("alter session set nls_date_format='YYYY-MM-DD HH24:MI:SS'");

	DA_FLUSH(DATA_FLUSH);

	return true;
}

void oci_dirver::close()
{
	OCILogoff(m_HandleMng.hp_ctx, m_HandleMng.hp_err);
	OCIServerDetach(m_HandleMng.hp_serv, m_HandleMng.hp_err, OCI_DEFAULT);
	m_HandleMng.FreeAll();
}

bool oci_dirver::execute(const char * sqlstr)
{
	if (!Prepare(sqlstr))
	{
		DA_ERRLOG("Oracle sql prepare error!");
		DA_FLUSH(ERROR_FLUSH);
		errmsg_="Oracle sql prepare error!";
		return false;
	}

	if (!Check(OCIStmtExecute(m_HandleMng.hp_ctx, m_HandleMng.hp_stmt, m_HandleMng.hp_err, (ub4)1, (ub4)0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT)))
	{
		DA_ERRLOG("Oracle sql execute error! %s" , sqlstr);
		DA_FLUSH(ERROR_FLUSH);
		errmsg_=("Oracle sql execute error! %s" , sqlstr);
		return false;
	}
	return true;
}

bool oci_dirver::query(db_result& result , const char * sqlstr , int limmited/* = -1*/)
{
	InitPrimaryKey();

	if (!Prepare(sqlstr))
	{
		DA_ERRLOG("Oracle sql prepare error! %s" , sqlstr);
		DA_FLUSH(ERROR_FLUSH);
		errmsg_=("Oracle sql prepare error! %s" , sqlstr); 
		return false;
	}

	sword swRet = 0;


	//先用OCI_DESCRIBE_ONLY获取返回的结构信息(列的信息)
	swRet = OCIStmtExecute(m_HandleMng.hp_ctx, m_HandleMng.hp_stmt, m_HandleMng.hp_err, (ub4)1, (ub4)0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DESCRIBE_ONLY);
	if (!Check(swRet))
	{
		DA_ERRLOG("Oracle sql pre_execute error! %s" , sqlstr);
		DA_FLUSH(ERROR_FLUSH);
		errmsg_=("Oracle sql pre_execute error! %s" , sqlstr);
		return false;
	}

	ss::db_result::header hTemp;
	if (!InitHeader(/*result.get_header()*/hTemp))
	{
		return false;
	}
	//采用set_header,因为会设置lower_case_header
	result.set_header(hTemp);

	swRet = OCIStmtExecute(m_HandleMng.hp_ctx, m_HandleMng.hp_stmt, m_HandleMng.hp_err, (ub4)1, (ub4)0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT);
	if (OCI_NO_DATA == swRet)
	{
		return true;
	}
	if (!Check(swRet))
	{
		DA_ERRLOG("Oracle sql execute error! %s" , sqlstr);
		DA_FLUSH(ERROR_FLUSH);
		errmsg_=("Oracle sql execute error! %s" , sqlstr);
		return false;
	}

	do 
	{
		AddRow(result);
		if (limmited > 0)
		{
			limmited --;
			if (0 == limmited)
			{
				break;
			}
		}
	} while (OCIStmtFetch2(m_HandleMng.hp_stmt, m_HandleMng.hp_err, 1, OCI_FETCH_NEXT, 1, OCI_DEFAULT) != OCI_NO_DATA);

	return true;
}

bool oci_dirver::begin()
{
	//if (!Check(OCIAttrSet((void *)m_HandleMng.hp_ctx, OCI_HTYPE_SVCCTX, (void *)m_HandleMng.hp_trans, 0, OCI_ATTR_TRANS, m_HandleMng.hp_err)))
	//{
	//	DA_ERRLOG("Oracle attribute set error!");
	//	DA_FLUSH(ERROR_FLUSH);
	//	errmsg_="Oracle attribute set error!";
	//	return false;
	//}

	//if (!Check(OCITransStart(m_HandleMng.hp_ctx, m_HandleMng.hp_err, 30, OCI_TRANS_NEW)))
	//{
	//	DA_ERRLOG("Oracle start error!");
	//	DA_FLUSH(ERROR_FLUSH);
	//	errmsg_="Oracle start error!";
	//	return false;
	//}
	return true;
}

bool oci_dirver::rollback()
{
	if (!Check(OCITransRollback(m_HandleMng.hp_ctx, m_HandleMng.hp_err, OCI_DEFAULT)))
	{
		DA_ERRLOG("Oracle rollback error!");
		DA_FLUSH(ERROR_FLUSH);
		errmsg_="Oracle rollback error!";
		return false;
	}
	return true;
}

bool oci_dirver::commit()
{
	if (!Check(OCITransCommit(m_HandleMng.hp_ctx, m_HandleMng.hp_err, OCI_DEFAULT)))
	{
		DA_ERRLOG("Oracle commit error!");
		DA_FLUSH(ERROR_FLUSH);
		errmsg_="Oracle commit error!";
		return false;
	}
	return true;
}

std::string& oci_dirver::real_escape_string(std::string &from) const
{
	replace_all(from, "'", "'||chr(39)||'");
	replace_all(from, "&", "'||chr(38)||'");
	return from;
}

void oci_dirver::ParseOptionalParam(const std::string& sOptionParam)
{
	std::string sTag = sOptionParam.substr(0, 2);
	if (sTag == "-t")
	{
		tablename_ = sOptionParam.substr(3, sOptionParam.length() - 3);
	}
	else if (sTag == "-r")
	{
		rolename_ = sOptionParam.substr(3, sOptionParam.length() - 3);
		boost::to_lower(rolename_);
	}
}

ub4 oci_dirver::GetConnMode()
{	//OCI_SYSOPER;
	//OCI_SYSDBA;
	//OCI_DEFAULT;
	ub4 Mode = OCI_DEFAULT;
	if (rolename_ == "sysdba")
	{
		Mode = OCI_SYSDBA;
	}
	else if (rolename_ == "sysoper")
	{
		Mode = OCI_SYSOPER;
	}
	return Mode;
}

//处理错误信息，status是OCI函数运行的返回值
bool oci_dirver::Check(sword status)
{
	text errbuf[512];
	sb4 errcode;
	if (status == OCI_ERROR)
	{
		if (m_HandleMng.hp_err)
		{
			// 获取错误指针和 OCI 错误代码
			OCIErrorGet((dvoid *)m_HandleMng.hp_err, (ub4)1, (text *)NULL, &errcode, errbuf, (ub4)sizeof(errbuf), OCI_HTYPE_ERROR);
			if (errcode == 1405)
			{
				//过滤1405
				return true;
			}
			//text errgbk[512];
			//size_t out;
			//OCINlsCharSetConvert(m_HandleMng.hp_env, m_HandleMng.hp_err, 852, errgbk, sizeof(errgbk), 871, errbuf, sizeof(errbuf), &out);
			//DA_ERRLOG("ERROR %d: %s" , errcode , errgbk);
			//errmsg_ = (char*)errgbk;
			DA_ERRLOG("ERROR %d: %s" , errcode , errbuf);
			errmsg_ = (char*)errbuf;
			errcode_ = errcode;
		}
		DA_FLUSH(ERROR_FLUSH);
		return false;
	}
	else if (status == OCI_INVALID_HANDLE)
	{
		DA_ERRLOG("INVALID HANDLE");
		DA_FLUSH(ERROR_FLUSH);
		return false;
	}

	return true;
}

bool oci_dirver::InitPrimaryKey()
{
	m_vctPK.clear();
	if (tablename_.empty())
	{
		return false;
	}
	
	std::string sSql = "SELECT A.COLUMN_NAME FROM USER_CONS_COLUMNS A, USER_CONSTRAINTS B WHERE A.CONSTRAINT_NAME = B.CONSTRAINT_NAME AND B.CONSTRAINT_TYPE = 'P' AND A.TABLE_NAME = '";
	sSql += tablename_;
	sSql += "'";


	if (!Prepare(sSql.c_str()))
	{
		return false;
	}

	sword swRet = 0;
	OCIDefine *hp_def;

	char buf[65];
	memset(&buf, 0, 65);
	swRet = OCIDefineByPos(m_HandleMng.hp_stmt, &hp_def, m_HandleMng.hp_err, 1, &buf, 64, SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT);
	if (!Check(swRet))
	{
		return false;
	}

	swRet = OCIStmtExecute(m_HandleMng.hp_ctx, m_HandleMng.hp_stmt, m_HandleMng.hp_err, (ub4)1, (ub4)0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT);
	if (OCI_NO_DATA == swRet)
	{
		return true;
	}
	if (!Check(swRet))
	{
		DA_ERRLOG("Oracle sql execute error!");
		DA_FLUSH(ERROR_FLUSH);
		errmsg_="Oracle sql execute error!";
		return false;
	}

	do 
	{
		std::string sPK = buf;
		m_vctPK.push_back(sPK);
	} while (OCIStmtFetch2(m_HandleMng.hp_stmt, m_HandleMng.hp_err, 1, OCI_FETCH_NEXT, 1, OCI_DEFAULT) != OCI_NO_DATA);

	return true;
}

bool oci_dirver::Prepare(const char * sqlstr)
{
	sword swRet = OCIStmtPrepare(m_HandleMng.hp_stmt, m_HandleMng.hp_err, (text *)sqlstr, (ub4)strlen(sqlstr), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
	if (!Check(swRet))
	{
		DA_ERRLOG("Query: %s\nCreate prepare error!" , sqlstr);
		DA_FLUSH(ERROR_FLUSH);
		return false;
	}

	return true;
}

bool oci_dirver::InitHeader(ss::db_result::header& refHead)
{
	//获取列数
	ub4 colnum = 0;
	sword swRet = OCIAttrGet((const dvoid *)m_HandleMng.hp_stmt, OCI_HTYPE_STMT, &colnum, 0 , OCI_ATTR_PARAM_COUNT, m_HandleMng.hp_err);
	if (!Check(swRet))
	{
		return false;
	}

	for (ub4 i = 0; i< colnum; ++i)
	{
		ub4 AttrSize = 0;
		swRet = OCIParamGet(m_HandleMng.hp_stmt, OCI_HTYPE_STMT, m_HandleMng.hp_err, (dvoid **)&m_HandleMng.hp_param, (ub4) i+1);


		oci_FieldInfo oci_FI;
		//swRet = OCIAttrGet((dvoid*)m_HandleMng.hp_param, OCI_DTYPE_PARAM,(dvoid*)&oci_FI.attr_is_null, &AttrSize, OCI_ATTR_IS_NULL,m_HandleMng.hp_err);
		swRet = OCIAttrGet((dvoid*)m_HandleMng.hp_param, OCI_DTYPE_PARAM,(dvoid*)&oci_FI.attr_name, &oci_FI.name_size, OCI_ATTR_NAME,m_HandleMng.hp_err);
		swRet = OCIAttrGet((dvoid*)m_HandleMng.hp_param, OCI_DTYPE_PARAM,(dvoid*)&oci_FI.attr_data_type, &AttrSize, OCI_ATTR_DATA_TYPE,m_HandleMng.hp_err);
		swRet = OCIAttrGet((dvoid*)m_HandleMng.hp_param, OCI_DTYPE_PARAM,(dvoid*)&oci_FI.attr_precision, &AttrSize, OCI_ATTR_PRECISION,m_HandleMng.hp_err);
		swRet = OCIAttrGet((dvoid*)m_HandleMng.hp_param, OCI_DTYPE_PARAM,(dvoid*)&oci_FI.attr_scale, &AttrSize, OCI_ATTR_SCALE,m_HandleMng.hp_err);
		swRet = OCIAttrGet((dvoid*)m_HandleMng.hp_param, OCI_DTYPE_PARAM,(dvoid*)&oci_FI.attr_data_size, &AttrSize, OCI_ATTR_DATA_SIZE,m_HandleMng.hp_err);

		if (oci_FI.IsNTY())
		{
			swRet = OCIAttrGet((dvoid*)m_HandleMng.hp_param, OCI_DTYPE_PARAM,(dvoid*)&oci_FI.attr_type_name, &oci_FI.type_name_size, OCI_ATTR_TYPE_NAME,m_HandleMng.hp_err);
			swRet = OCIAttrGet((dvoid*)m_HandleMng.hp_param, OCI_DTYPE_PARAM,(dvoid*)&oci_FI.attr_schema_name, &oci_FI.schema_name_size, OCI_ATTR_SCHEMA_NAME,m_HandleMng.hp_err);
		}
		
		OCIDefine *hp_def = NULL;
		oci_FieldDataBuf *pFBuf = oci_FieldDataBuf::Create(oci_FI);
		pFBuf->SetHandleMng(&m_HandleMng);
		swRet = OCIDefineByPos(m_HandleMng.hp_stmt, &hp_def, m_HandleMng.hp_err, i+1, pFBuf->GetBuf(), oci_FI.GetDefineSize(), oci_FI.GetDefineType(), NULL, NULL, NULL, OCI_DEFAULT);
		if (!Check(swRet))
		{
			continue;
		}

		if (oci_FI.IsNTY())
		{
			OCIType *pOCIType;
			oci_FieldDataBuf_NTY *pFBuf_NTY = (oci_FieldDataBuf_NTY*)pFBuf;
			swRet = OCITypeByName(m_HandleMng.hp_env, m_HandleMng.hp_err,m_HandleMng.hp_ctx, oci_FI.attr_schema_name, oci_FI.schema_name_size,	
				oci_FI.attr_type_name, oci_FI.type_name_size, (text *)0, 0, OCI_DURATION_SESSION, OCI_TYPEGET_HEADER, &pOCIType);
			if (!Check(swRet))
			{
				continue;
			}
			swRet = OCIDefineObject(hp_def, m_HandleMng.hp_err, pOCIType, pFBuf_NTY->GetAddr(), (ub4 *) 0, (void  **) 0, (ub4 *) 0);
			pFBuf_NTY->SetType(pOCIType);
		}

		m_RowBuff.m_vctFDataBuf.push_back(pFBuf);

		ss::field_def fieldInfo;
		oci_FI.SetToSSFieldDef(fieldInfo);
		fieldInfo.table_name = tablename_;
		refHead.add(fieldInfo);
	}

	for (std::vector<std::string>::iterator iter = m_vctPK.begin(); iter != m_vctPK.end(); ++iter)
	{
		refHead.add_primary(*iter);
	}
	return true;
}

bool oci_dirver::AddRow(ss::db_result& result)
{
	int nfc = result.get_header().field_count();
	int size = sizeof(ss::field) * nfc;
	int BlkLen = size;
	ss::block * block = new ss::block(BlkLen);
	char * buffer = (char *)block->buffer() ;
	if(NULL == buffer)
	{
		return false;
	}
	memset(buffer , 0 , BlkLen);
	//buffer是block中 head_+offset_，即block中有效数据的指针
	ss::__field_holder *fields = (ss::__field_holder *)buffer ;
	for(int idx = 0 ; idx < nfc ; ++idx)
	{
		//注意区分header.fields(idx)和fieldss[idx]的类型
		int type = result.get_header().fields(idx).type ;
		int ndatasize = result.get_header().fields(idx).max_length;

		fields[idx].type = type ;
		fields[idx].magic = 0x5555 ;
		fields[idx].length = ss::get_type_size(type) ;
		fields[idx].data.val = 0 ;

		m_RowBuff.m_vctFDataBuf[idx]->Transe(fields[idx]);
	}

	block->length(BlkLen) ;
	ss::db_row * db_row = new ss::db_row(block) ;

	db_row->attach(block) ;

	result.add_row(db_row) ;
	return true;
}
#endif

