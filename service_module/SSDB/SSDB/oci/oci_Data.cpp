#if defined(OCI_DRIVER)

#include "oci_Data.h"
#include "oci_HandleMng.h"

using namespace ss;

/////////////////////////class oci_FieldInfo/////////////////////////
oci_FieldInfo::oci_FieldInfo()
:attr_name(NULL)
,name_size(0)
,attr_data_type(0)
,attr_precision(0)
,attr_scale(0)
,attr_data_size(0)
,attr_type_name(NULL)
,type_name_size(0)
,attr_schema_name(NULL)
,schema_name_size(0)
{

}

oci_FieldInfo::~oci_FieldInfo()
{

}

void oci_FieldInfo::SetToSSFieldDef(ss::field_def& ssField)
{
	std::string sTmpName = (char*)attr_name;
	ssField.name = sTmpName.substr(0, name_size);
	ssField.length = attr_data_size;
	ssField.max_length = attr_data_size;
	ssField.type = DataTypeAdapt();
	ssField.decimals = attr_scale;
}

ss::uint8 oci_FieldInfo::DataTypeAdapt() const
{
	switch(attr_data_type)
	{
	case SQLT_CHR:
	case SQLT_STR:
	case SQLT_AFC:
		return ss::SSTYPE_STRING;
	case SQLT_DATE:
	case SQLT_DAT:
		return ss::SSTYPE_T32;
	case SQLT_TIMESTAMP:
	case SQLT_TIMESTAMP_TZ:
	case SQLT_TIMESTAMP_LTZ:
		return ss::SSTYPE_T64;
	case  SQLT_BIN:
		return ss::SSTYPE_RAW;
	case SQLT_NUM:
		return AdaptNUMSubType();
	case SQLT_FLT:
		return ss::SSTYPE_FLOAT;
	case SQLT_INT:
		return ss::SSTYPE_INT32;
	case SQLT_UIN:
		return ss::SSTYPE_UINT32;
	case SQLT_NTY:
		return ss::SSTYPE_RAW;
	case SQLT_CLOB:
	case SQLT_BLOB:
		return ss::SSTYPE_STRING;
	case SQLT_INTERVAL_YM:
	case SQLT_INTERVAL_DS:
	case SQLT_PDN:
	case SQLT_REF:
	case SQLT_VST:
	case SQLT_VNU:
	default:
		return ss::SSTYPE_UNKNOWN;
	}
}

ub2 oci_FieldInfo::GetDefineType()
{
	switch(attr_data_type)
	{
	case SQLT_CHR:
	case SQLT_STR:
	case SQLT_AFC:
		return SQLT_STR;
	case SQLT_DATE:
	case SQLT_DAT:
		return SQLT_DAT;
	case SQLT_TIMESTAMP:
	case SQLT_TIMESTAMP_TZ:
	case SQLT_TIMESTAMP_LTZ:
		return SQLT_STR;
	case SQLT_BIN:
		return SQLT_BIN;
	case SQLT_NTY:
		return SQLT_NTY;
	case SQLT_NUM:
	case SQLT_FLT:
	case SQLT_INT:
	case SQLT_UIN:
		return SQLT_STR;
	case SQLT_CLOB:
	case SQLT_BLOB:
		return SQLT_LNG;
	case SQLT_INTERVAL_YM:
	case SQLT_INTERVAL_DS:
	case SQLT_PDN:
	case SQLT_REF:
	case SQLT_VST:
	case SQLT_VNU:
	default:
		return attr_data_type;
	}
}

ub4 oci_FieldInfo::GetDefineSize() const
{
	switch(attr_data_type)
	{
	case SQLT_NTY:
		return 0;
	case SQLT_TIMESTAMP:
		return DATETIMELOBSIZE;
	case SQLT_CLOB:
		return LOBSIZE;
	default:
		return attr_data_size*3;
	}
}

bool oci_FieldInfo::IsNTY()
{
	return attr_data_type == SQLT_NTY;
}

ss::uint8 oci_FieldInfo::AdaptNUMSubType() const
{
	//Oracle内部使用Number类型,这里根据scale和precision区分double, int64和int32
	if (attr_scale > 0)
	{
		//有小数位的处理为double
		return ss::SSTYPE_DOUBLE;
	}
	if (attr_precision > 9)
	{
		//-10^9~10^9之外的处理为int64
		return ss::SSTYPE_INT64;
	}
	//-10^9~10^9之间的处理为int32
	return ss::SSTYPE_INT32;
}


/////////////////////////class oci_FieldDataBuf/////////////////////////
oci_FieldDataBuf::oci_FieldDataBuf()
:m_pHMng(NULL)
{
}

oci_FieldDataBuf::~oci_FieldDataBuf()
{
}

oci_FieldDataBuf* oci_FieldDataBuf::Create(const oci_FieldInfo& fi)
{
	switch(fi.attr_data_type)
	{
	case SQLT_CHR:
	case SQLT_STR:
	case SQLT_AFC:
		return new oci_FieldDataBuf_CHR(fi.attr_data_size*3);
	case SQLT_DATE:
	case SQLT_DAT:
		return new oci_FieldDataBuf_DAT();
	case SQLT_NUM:
		return new oci_FieldDataBuf_NUM(fi);
	case SQLT_TIMESTAMP:
	case SQLT_TIMESTAMP_TZ:
	case SQLT_TIMESTAMP_LTZ:
		return new oci_FieldDataBuf_DateTime();
	case SQLT_BIN:
		return new oci_FieldDataBuf_RAW(fi.attr_data_size);
	case SQLT_NTY:
		return new oci_FieldDataBuf_NTY();
	case SQLT_CLOB:
	case SQLT_BLOB:
		return new oci_FieldDataBuf_LOB();
	case SQLT_INTERVAL_YM:
	case SQLT_INTERVAL_DS:
	case SQLT_INT:
	case SQLT_UIN:
	case SQLT_FLT:
	case SQLT_PDN:
	case SQLT_REF:
	case SQLT_VST:
	case SQLT_VNU:
	default:
		return new oci_FieldDataBuf_CHR(fi.attr_data_size*3);
	}
}

void* oci_FieldDataBuf::GetBuf()
{
	return NULL;
}

void oci_FieldDataBuf::Transe(__field_holder& TranseDest)
{
}

void oci_FieldDataBuf::SetHandleMng(oci_HandleMng* pHMng)
{
	m_pHMng = pHMng;
}

bool oci_FieldDataBuf::CheckValIsNull()
{
	size_t bufsize = ::strlen((char*)GetBuf());
	return bufsize == 0;
}

/////////////////////////class oci_FieldDataBuf_NUM/////////////////////////
oci_FieldDataBuf_NUM::oci_FieldDataBuf_NUM(const oci_FieldInfo& fi)
{
	m_pBuf = new char[fi.GetDefineSize() + 1];
	memset(m_pBuf, 0, fi.GetDefineSize() + 1);
	m_AdaptType = fi.DataTypeAdapt();
}

oci_FieldDataBuf_NUM::~oci_FieldDataBuf_NUM()
{
	if (m_pBuf)
	{
		delete[] m_pBuf;
		m_pBuf = NULL;
	}
}

void* oci_FieldDataBuf_NUM::GetBuf()
{
	return m_pBuf;
}

void oci_FieldDataBuf_NUM::Transe(__field_holder& TranseDest)
{
	if (CheckValIsNull())
	{
		ss::set_bit_mask(TranseDest.flag , 0 , false);
		return;
	}
	if (ss::SSTYPE_DOUBLE == m_AdaptType)
	{
		TranseDest.data.f64 = (double)atof(m_pBuf) ;
	}
	else if (ss::SSTYPE_INT64 == m_AdaptType)
	{
		TranseDest.data.i64 = (ss::int64)atoi64(m_pBuf) ;
	}
	else if (ss::SSTYPE_INT32 == m_AdaptType)
	{
		TranseDest.data.i32 = (ss::int32)atoi(m_pBuf) ;
	}
	ss::set_bit_mask(TranseDest.flag , 0 , true);
}

/////////////////////////class oci_FieldDataBuf_CHR/////////////////////////

oci_FieldDataBuf_CHR::oci_FieldDataBuf_CHR(size_t szBuf)
{
	m_pBuf = new char[szBuf + 1];
	memset(m_pBuf, 0, szBuf + 1);
}

oci_FieldDataBuf_CHR::~oci_FieldDataBuf_CHR()
{
	if (m_pBuf)
	{
		delete[] m_pBuf;
		m_pBuf = NULL;
	}
}

void* oci_FieldDataBuf_CHR::GetBuf()
{
	return m_pBuf;
}

void oci_FieldDataBuf_CHR::Transe(__field_holder& TranseDest)
{
	if (CheckValIsNull())
	{
		ss::set_bit_mask(TranseDest.flag , 0 , false);
		return;
	}

	bool assigned = true;

	int strsize = ::strlen(m_pBuf) + 1;
	char * pchar = (char *)malloc(strsize);
	if(NULL == pchar)
	{
		assigned = false ;
	}
	else
	{
		memcpy(pchar, m_pBuf , strsize) ;
		TranseDest.data.str = pchar;
		TranseDest.length = strsize;
	}

	ss::set_bit_mask(TranseDest.flag , 0 , assigned);
}

/////////////////////////class oci_FieldDataBuf_DAT/////////////////////////
oci_FieldDataBuf_DAT::oci_FieldDataBuf_DAT()
{
	//memset(&m_Data, 0, sizeof(OCIDate));
	m_pBuf = new char(sizeof(OCIDate));
	memset(m_pBuf, 0, sizeof(OCIDate));
}

oci_FieldDataBuf_DAT::~oci_FieldDataBuf_DAT()
{
}

void* oci_FieldDataBuf_DAT::GetBuf()
{
	//return &m_Data;
	return m_pBuf;
}

int TranseOraYear(sb2 in)
{
	int a = (in & 0x00ff) - 0x64;
	int b = ((in & 0xff00) >> 8) - 0x64;
	return a*100 + b;
}

void oci_FieldDataBuf_DAT::Transe(__field_holder& TranseDest)
{
	if (CheckValIsNull())
	{
		ss::set_bit_mask(TranseDest.flag , 0 , false);
		return;
	}

	OCIDate DateTmp;
	memcpy(&DateTmp, m_pBuf, sizeof(OCIDate));

	int year = TranseOraYear(DateTmp.OCIDateYYYY);
	TranseDest.data.t32 = ss::make_time(year, DateTmp.OCIDateMM, DateTmp.OCIDateDD, DateTmp.OCIDateTime.OCITimeHH - 1, DateTmp.OCIDateTime.OCITimeMI - 1, DateTmp.OCIDateTime.OCITimeSS - 1);
	ss::set_bit_mask(TranseDest.flag , 0 , true);
	memset(m_pBuf, 0, sizeof(OCIDate));
}

/////////////////////////oci_FieldDataBuf_DateTime/////////////////////////
oci_FieldDataBuf_DateTime::oci_FieldDataBuf_DateTime()
{
	memset(buffer, 0, DATETIMELOBSIZE);
}

oci_FieldDataBuf_DateTime::~oci_FieldDataBuf_DateTime()
{

}

void* oci_FieldDataBuf_DateTime::GetBuf()
{
	return &buffer;
}

void oci_FieldDataBuf_DateTime::Transe(ss::__field_holder& TranseDest)
{
	if (CheckValIsNull())
	{
		ss::set_bit_mask(TranseDest.flag , 0 , false);
		return;
	}

	OCIDateTime *pTime;
	sword sw = OCIDescriptorAlloc(m_pHMng->hp_env, (void**)&pTime, OCI_DTYPE_TIMESTAMP, 0, NULL);

	sw = OCIDateTimeFromText(m_pHMng->hp_env, m_pHMng->hp_err, buffer, DATETIMELOBSIZE, NULL, 0, NULL, 0, pTime);
	sb2 year;
	ub1 month;
	ub1 day;
	ub1 hour;
	ub1 min;
	ub1 sec;
	ub4 fsec;
	sw = OCIDateTimeGetDate(m_pHMng->hp_env, m_pHMng->hp_err, pTime, &year, &month, &day);
	sw = OCIDateTimeGetTime(m_pHMng->hp_env, m_pHMng->hp_err, pTime, &hour, &min, &sec, &fsec);
	OCIDescriptorFree(pTime, OCI_DTYPE_TIMESTAMP);
	ub4 mesc = fsec/(1000*1000);
	TranseDest.data.t64 = (ss::int32)ss::make_time(year, month, day, hour, min, sec);
	TranseDest.data.t64 *= 1000;
	TranseDest.data.t64 += mesc;

	ss::set_bit_mask(TranseDest.flag , 0 , true);
}

/////////////////////////oci_FieldDataBuf_LOB/////////////////////////
oci_FieldDataBuf_LOB::oci_FieldDataBuf_LOB()
{
	memset(buffer, 0, LOBSIZE);
}

oci_FieldDataBuf_LOB::~oci_FieldDataBuf_LOB()
{

}

void* oci_FieldDataBuf_LOB::GetBuf()
{
	return &buffer;
}

void oci_FieldDataBuf_LOB::Transe(ss::__field_holder& TranseDest)
{
	if (CheckValIsNull())
	{
		ss::set_bit_mask(TranseDest.flag , 0 , false);
		return;
	}

	bool assigned = true;
	int strsize = ::strlen((char*)&buffer) + 1;
	char * pchar = (char *)malloc(strsize);
	if(NULL == pchar)
	{
		assigned = false ;
	}
	else
	{
		memcpy(pchar, &buffer , strsize) ;
		TranseDest.data.str = pchar;
		TranseDest.length = strsize;
	}

	ss::set_bit_mask(TranseDest.flag , 0 , assigned);

	memset(buffer, 0, LOBSIZE);
}


/////////////////////////class oci_FieldDataBuf_RAW/////////////////////////
oci_FieldDataBuf_RAW::oci_FieldDataBuf_RAW(size_t szBuf)
{
	m_pBuf = new char[szBuf];
	memset(m_pBuf, 0, szBuf);
	m_szDataSize = szBuf;
}
oci_FieldDataBuf_RAW::~oci_FieldDataBuf_RAW()
{
	if (m_pBuf)
	{
		delete[] m_pBuf;
		m_pBuf = NULL;
	}
}

void* oci_FieldDataBuf_RAW::GetBuf()
{
	return m_pBuf;
}

void oci_FieldDataBuf_RAW::Transe(ss::__field_holder& TranseDest)
{
	if (CheckValIsNull())
	{
		ss::set_bit_mask(TranseDest.flag , 0 , false);
		return;
	}

	bool assigned = true;

	char * pch = (char *)malloc(m_szDataSize);
	if(NULL == pch)
	{
		assigned = false;
	}
	else
	{
		memcpy(pch, m_pBuf, m_szDataSize);
		TranseDest.data.raw = pch;
		TranseDest.length = m_szDataSize;
	}

	ss::set_bit_mask(TranseDest.flag , 0 , assigned);
}

/////////////////////////class oci_FieldDataBuf_NTY/////////////////////////
oci_FieldDataBuf_NTY::oci_FieldDataBuf_NTY()
:m_pAddr(NULL)
,m_pOCIType(NULL)
{

}

oci_FieldDataBuf_NTY::~oci_FieldDataBuf_NTY()
{

}

void* oci_FieldDataBuf_NTY::GetBuf()
{
	return 0;
}

void** oci_FieldDataBuf_NTY::GetAddr()
{
	return &m_pAddr;
}

void oci_FieldDataBuf_NTY::SetType(OCIType* pType)
{
	m_pOCIType = pType;
}

void oci_FieldDataBuf_NTY::Transe(ss::__field_holder& TranseDest)
{
	//暂不支持
	if (CheckValIsNull())
	{
		ss::set_bit_mask(TranseDest.flag , 0 , false);
		return;
	}
}


/////////////////////////class oci_RowDataBuf/////////////////////////

oci_RowDataBuf::oci_RowDataBuf()
{

}

oci_RowDataBuf::~oci_RowDataBuf()
{
	ClearRowBuf();
}

void oci_RowDataBuf::ClearRowBuf()
{
	for (vctFDataBuf::iterator iter = m_vctFDataBuf.begin(); iter != m_vctFDataBuf.end(); ++iter)
	{
		if (*iter)
		{
			delete (*iter);
		}
	}
	m_vctFDataBuf.clear();
}
#endif
