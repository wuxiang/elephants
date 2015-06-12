#ifndef __OCI_DATA_H
#define __OCI_DATA_H

#include "SSDBDriver.h"
#include "include/oci.h"

#define  DATETIMELOBSIZE 128
#define  LOBSIZE 8000

class oci_FieldInfo
{
public:
	oci_FieldInfo();
	~oci_FieldInfo();

	void SetToSSFieldDef(ss::field_def& ssField);
	ss::uint8 DataTypeAdapt() const;
	//查询绑定时使用的类型
	ub2 GetDefineType();
	//查询绑定时的长度
	ub4 GetDefineSize() const;

	bool IsNTY();

protected:
	ss::uint8 AdaptNUMSubType() const;

public:
	//OCI_ATTR_NAME
	oratext* attr_name;
	ub4 name_size;
	//OCI_ATTR_DATA_TYPE
	ub2 attr_data_type;
	//OCI_ATTR_PRECISION
	ub1 attr_precision;
	//OCI_ATTR_SCALE
	sb1 attr_scale;
	//OCI_ATTR_DATA_SIZE
	ub4 attr_data_size;
	//OCI_ATTR_IS_NULL
	//ub1 attr_is_null;

	//SQLT_NTY
	oratext* attr_type_name;
	ub4 type_name_size;
	oratext* attr_schema_name;
	ub4 schema_name_size;
};

class oci_HandleMng;

class oci_FieldDataBuf
{
protected:
	oci_FieldDataBuf();
public:
	virtual ~oci_FieldDataBuf();
	static oci_FieldDataBuf* Create(const oci_FieldInfo& fi);
	virtual void* GetBuf();
	virtual void Transe(ss::__field_holder& TranseDest);
	void SetHandleMng(oci_HandleMng* pHMng);
	bool CheckValIsNull();

protected:
	oci_HandleMng* m_pHMng;
};

class oci_FieldDataBuf_NUM : public oci_FieldDataBuf
{
public:
	oci_FieldDataBuf_NUM(const oci_FieldInfo& fi);
	~oci_FieldDataBuf_NUM();
	void* GetBuf();
	void Transe(ss::__field_holder& TranseDest);

protected:
	char* m_pBuf;
	ss::uint8 m_AdaptType;
};

class oci_FieldDataBuf_CHR : public oci_FieldDataBuf
{
public:
	oci_FieldDataBuf_CHR(size_t szBuf);
	~oci_FieldDataBuf_CHR();
	void* GetBuf();
	void Transe(ss::__field_holder& TranseDest);

protected:
	char* m_pBuf;
};

class oci_FieldDataBuf_DAT : public oci_FieldDataBuf
{
public:
	oci_FieldDataBuf_DAT();
	~oci_FieldDataBuf_DAT();
	void* GetBuf();
	void Transe(ss::__field_holder& TranseDest);

protected:
	//OCIDate m_Data;
	char* m_pBuf;
};

class oci_FieldDataBuf_DateTime : public oci_FieldDataBuf
{
public:
	oci_FieldDataBuf_DateTime();
	~oci_FieldDataBuf_DateTime();
	void* GetBuf();
	void Transe(ss::__field_holder& TranseDest);

protected:
	//OCIDateTime* m_pData;
	ub1 buffer[DATETIMELOBSIZE];
};

class oci_FieldDataBuf_LOB : public oci_FieldDataBuf
{
public:
	oci_FieldDataBuf_LOB();
	~oci_FieldDataBuf_LOB();
	void* GetBuf();
	void Transe(ss::__field_holder& TranseDest);

protected:
	ub1 buffer[LOBSIZE];
};

class oci_FieldDataBuf_RAW : public oci_FieldDataBuf
{
public:
	oci_FieldDataBuf_RAW(size_t szBuf);
	~oci_FieldDataBuf_RAW();
	void* GetBuf();
	void Transe(ss::__field_holder& TranseDest);

protected:
	char* m_pBuf;
	size_t m_szDataSize;
};

class oci_FieldDataBuf_NTY : public oci_FieldDataBuf
{
public:
	oci_FieldDataBuf_NTY();
	~oci_FieldDataBuf_NTY();
	void* GetBuf();
	void** GetAddr();
	void SetType(OCIType* pType);
	void Transe(ss::__field_holder& TranseDest);

protected:
	OCIType* m_pOCIType;
	void* m_pAddr;
};

typedef std::vector<oci_FieldDataBuf*> vctFDataBuf;

class oci_RowDataBuf
{
public:
	oci_RowDataBuf();
	~oci_RowDataBuf();

	void ClearRowBuf();

public:
	vctFDataBuf m_vctFDataBuf;
};

#endif /** __OCI_DATA_H */
