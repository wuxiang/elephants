#if defined(OCI_DRIVER)

#include "oci_HandleMng.h"

/////////////////////////class oci_HandleMng/////////////////////////
oci_HandleMng::oci_HandleMng()
:hp_env(NULL)
,hp_err(NULL)
,hp_serv(NULL)
,hp_auth(NULL)
,hp_ctx(NULL)
,hp_stmt(NULL)
,hp_param(NULL)
{

}

oci_HandleMng::~oci_HandleMng()
{
	FreeAll();
}

void oci_HandleMng::InitAll()
{
	OCIHandleAlloc(hp_env, (dvoid **)&hp_err, OCI_HTYPE_ERROR, 0, NULL);
	OCIHandleAlloc(hp_env, (dvoid **)&hp_serv, OCI_HTYPE_SERVER, 0, NULL);
	OCIHandleAlloc(hp_env, (dvoid **)&hp_auth, OCI_HTYPE_SESSION, 0, NULL);
	OCIHandleAlloc(hp_env, (dvoid **)&hp_ctx, OCI_HTYPE_SVCCTX, 0, NULL);
	OCIHandleAlloc(hp_env, (dvoid **)&hp_trans, OCI_HTYPE_TRANS, 0, NULL);
	OCIHandleAlloc(hp_env, (dvoid **)&hp_stmt, OCI_HTYPE_STMT, 0, NULL);
}

void oci_HandleMng::FreeAll()
{
	if (hp_param)
	{
		OCIHandleFree((dvoid *)hp_param, OCI_HTYPE_TRANS);
		hp_param = NULL;
	}
	if (hp_stmt)
	{
		OCIHandleFree((dvoid *)hp_stmt, OCI_HTYPE_STMT);
		hp_stmt = NULL;
	}
	if (hp_ctx)
	{
		OCIHandleFree((dvoid *)hp_ctx, OCI_HTYPE_SVCCTX);
		hp_ctx = NULL;
	}
	if (hp_auth)
	{
		OCIHandleFree((dvoid *)hp_auth, OCI_HTYPE_SESSION);
		hp_auth = NULL;
	}
	if (hp_serv)
	{
		OCIHandleFree((dvoid *)hp_serv, OCI_HTYPE_SERVER);
		hp_serv = NULL;
	}
	if (hp_err)
	{
		OCIHandleFree((dvoid *)hp_err, OCI_HTYPE_ERROR);
		hp_err = NULL;
	}
	if (hp_env)
	{
		OCIHandleFree((dvoid *)hp_env, OCI_HTYPE_ENV);
		hp_env = NULL;
	}
}
#endif
