#ifndef __OCI_HANDLEMNG_H
#define __OCI_HANDLEMNG_H

#include "include/oci.h"

class oci_HandleMng
{
public:
	oci_HandleMng();
	~oci_HandleMng();

	void InitAll();
	void FreeAll();

public:
	OCIEnv* hp_env;
	OCIError* hp_err;
	OCIServer* hp_serv;
	OCISession* hp_auth;
	OCISvcCtx* hp_ctx;
	OCITrans* hp_trans;
	OCIStmt* hp_stmt;
	OCIParam* hp_param;
};


#endif /** __OCI_HANDLEMNG_H */
