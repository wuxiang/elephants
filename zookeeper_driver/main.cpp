#include <iostream>
#include <stdio.h>
#include "zookeeper.h"

//const int ZOO_EXPIRED_SESSION_STATE = EXPIRED_SESSION_STATE_DEF;
//const int ZOO_AUTH_FAILED_STATE = AUTH_FAILED_STATE_DEF;
//const int ZOO_CONNECTING_STATE = CONNECTING_STATE_DEF;
//const int ZOO_ASSOCIATING_STATE = ASSOCIATING_STATE_DEF;
//const int ZOO_CONNECTED_STATE = CONNECTED_STATE_DEF;
zhandle_t* zk;
static const char* state2String(int state){
  if (state == 0)
    return "CLOSED_STATE";
  if (state == ZOO_CONNECTING_STATE)
    return "CONNECTING_STATE";
  if (state == ZOO_ASSOCIATING_STATE)
    return "ASSOCIATING_STATE";
  if (state == ZOO_CONNECTED_STATE)
    return "CONNECTED_STATE";
  if (state == ZOO_EXPIRED_SESSION_STATE)
    return "EXPIRED_SESSION_STATE";
  if (state == ZOO_AUTH_FAILED_STATE)
    return "AUTH_FAILED_STATE";

  return "INVALID_STATE";
}

static const char* type2String(int state){
  if (state == ZOO_CREATED_EVENT)
    return "CREATED_EVENT";
  if (state == ZOO_DELETED_EVENT)
    return "DELETED_EVENT";
  if (state == ZOO_CHANGED_EVENT)
    return "CHANGED_EVENT";
  if (state == ZOO_CHILD_EVENT)
    return "CHILD_EVENT";
  if (state == ZOO_SESSION_EVENT)
    return "SESSION_EVENT";
  if (state == ZOO_NOTWATCHING_EVENT)
    return "NOTWATCHING_EVENT";

  return "UNKNOWN_EVENT_TYPE";
}

static clientid_t myid;
static const char *clientIdFile = "myid";

void watcher(zhandle_t *zzh, int type, int state, const char *path, void* context)
{
    /* Be careful using zh here rather than zzh - as this may be mt code
     * the client lib may call the watcher before zookeeper_init returns */
	fprintf(stderr, "\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    fprintf(stderr, "Watcher %s state = %s", type2String(type), state2String(state));
    if (path && strlen(path) > 0) {
      fprintf(stderr, " for path %s", path);
    }
    fprintf(stderr, "\n");

    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            const clientid_t *id = zoo_client_id(zzh);
            if (myid.client_id == 0 || myid.client_id != id->client_id) {
                myid = *id;
                fprintf(stderr, "Got a new session id: 0x%llx\n", (long long)( myid.client_id));
                if (clientIdFile) {
                    FILE *fh = fopen(clientIdFile, "w");
                    if (!fh) {
                        perror(clientIdFile);
                    } else {
                        int rc = fwrite(&myid, sizeof(myid), 1, fh);
                        if (rc != sizeof(myid)) {
                            perror("writing client id");
                        }
                        fclose(fh);
                    }
                }
            }
        } else if (state == ZOO_AUTH_FAILED_STATE) {
            fprintf(stderr, "Authentication failure. Shutting down...\n");
            zookeeper_close(zzh);
        } else if (state == ZOO_EXPIRED_SESSION_STATE) {
            fprintf(stderr, "Session expired. Shutting down...\n");
            zookeeper_close(zzh);
        }
    }

	if (type == ZOO_CHANGED_EVENT)
	{
		fprintf(stderr, "node data change event\n");
        if (state == ZOO_CONNECTED_STATE)
		{
            const clientid_t *id = zoo_client_id(zzh);
            if (myid.client_id == 0 || myid.client_id != id->client_id)
			{
                myid = *id;
                fprintf(stderr, "Got a new session id: 0x%llx\n", (long long)( myid.client_id));
                if (clientIdFile)
				{
                    FILE *fh = fopen(clientIdFile, "w");
                    if (!fh)
					{
                        perror(clientIdFile);
                    } else
					{
                        int rc = fwrite(&myid, sizeof(myid), 1, fh);
                        if (rc != sizeof(myid))
						{
                            perror("writing client id");
                        }
                        fclose(fh);
                    }
                }
            }
        } else if (state == ZOO_AUTH_FAILED_STATE)
		{
            fprintf(stderr, "Authentication failure. Shutting down...\n");
            zookeeper_close(zzh);
        } else if (state == ZOO_EXPIRED_SESSION_STATE)
		{
            fprintf(stderr, "Session expired. Shutting down...\n");
            zookeeper_close(zzh);
        }
	}

	if (ZOO_DELETED_EVENT == type)
	{
		fprintf(stderr, "delete node event\n");
        if (state == ZOO_CONNECTED_STATE)
		{
            const clientid_t *id = zoo_client_id(zzh);
            if (myid.client_id == 0 || myid.client_id != id->client_id)
			{
                myid = *id;
                fprintf(stderr, "Got a new session id: 0x%llx\n", (long long)( myid.client_id));
                if (clientIdFile)
				{
                    FILE *fh = fopen(clientIdFile, "w");
                    if (!fh)
					{
                        perror(clientIdFile);
                    } else
					{
                        int rc = fwrite(&myid, sizeof(myid), 1, fh);
                        if (rc != sizeof(myid))
						{
                            perror("writing client id");
                        }
                        fclose(fh);
                    }
                }
            }
        } else if (state == ZOO_AUTH_FAILED_STATE)
		{
            fprintf(stderr, "Authentication failure. Shutting down...\n");
            zookeeper_close(zzh);
        } else if (state == ZOO_EXPIRED_SESSION_STATE)
		{
            fprintf(stderr, "Session expired. Shutting down...\n");
            zookeeper_close(zzh);
        }
	}

	if (ZOO_CHILD_EVENT == type)
	{
		fprintf(stderr, "child node event\n");
        if (state == ZOO_CONNECTED_STATE)
		{
			const clientid_t *id = zoo_client_id(zzh);
			if (myid.client_id == 0 || myid.client_id != id->client_id)
			{
				myid = *id;
				fprintf(stderr, "Got a new session id: 0x%llx\n", (long long)( myid.client_id));
				if (clientIdFile)
				{
					FILE *fh = fopen(clientIdFile, "w");
					if (!fh)
					{
						perror(clientIdFile);
					} else
					{
						int rc = fwrite(&myid, sizeof(myid), 1, fh);
						if (rc != sizeof(myid))
						{
							perror("writing client id");
						}
						fclose(fh);
					}
				}
			}

			String_vector cRes;
			if (ZOK != zoo_get_children(zk, "/zk_test", 1,  &cRes))
			{
				fprintf(stderr, "set node value failed!!!\n");
				zookeeper_close(zk);
				exit(0);
			}
        } else if (state == ZOO_AUTH_FAILED_STATE)
		{
            fprintf(stderr, "Authentication failure. Shutting down...\n");
            zookeeper_close(zzh);
        } else if (state == ZOO_EXPIRED_SESSION_STATE)
		{
            fprintf(stderr, "Session expired. Shutting down...\n");
            zookeeper_close(zzh);
        }
	}
	fprintf(stderr, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

static const char* const week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char* const month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static std::string convert(time_t time)
{
	tm timeinfo = { 0 };
	if (NULL == localtime_r(&time, &timeinfo))
	{
		return std::string();
	}

	char ctime[50] = { 0 };
	sprintf(ctime, "%s %s %d %d:%d:%d CST %d",
			week[timeinfo.tm_wday],
			month[timeinfo.tm_mon],
			timeinfo.tm_mday,
			timeinfo.tm_hour,
			timeinfo.tm_min,
			timeinfo.tm_sec,
			timeinfo.tm_year + 1900);

	return std::string(ctime);
}


void watchf(zhandle_t *zh, int type, int state, const char *path,void *watcherCtx)
{
}

void string_completion(int rc, const char *value, const void *data)
{
	fprintf(stderr, "[%s]: rc = %d", (char*)data, rc);
	if (value)
	{
		fprintf(stderr, ", value=[%s]\n", value);
	}
}

void stat_completion(int rc, const struct Stat *stat, const void *data)
{
	fprintf(stderr, "[%s]: rc = %d\n", (char*)data, rc);
	fprintf(stderr, "czxid = 0x%llx\n", stat->czxid);
	fprintf(stderr, "mzxid = 0x%llx\n", stat->mzxid);
	fprintf(stderr, "ctime = %s\n", convert(stat->ctime / 1000).c_str());
	fprintf(stderr, "mtime = %s\n", convert(stat->mtime / 1000).c_str());
	fprintf(stderr, "version = %ld\n", stat->version);
	fprintf(stderr, "cversion = %ld\n", stat->cversion);
	fprintf(stderr, "aversion = %ld\n", stat->aversion);
	fprintf(stderr, "ephemeralOwner = 0x%llx\n", stat->ephemeralOwner);
	fprintf(stderr, "dataLength = %ld\n", stat->dataLength);
	fprintf(stderr, "numChildren = %ld\n", stat->numChildren);
	fprintf(stderr, "pzxid = 0x%llx\n", stat->pzxid);
}

void data_completion(int rc, const char *value, int value_len, const struct Stat *stat, const void *data)
{
	fprintf(stderr, "[%s]: rc = %d, value = %s\n", (char*)data, rc, value);
	fprintf(stderr, "czxid = 0x%llx\n", stat->czxid);
	fprintf(stderr, "mzxid = 0x%llx\n", stat->mzxid);
	fprintf(stderr, "ctime = %s\n", convert(stat->ctime / 1000).c_str());
	fprintf(stderr, "mtime = %s\n", convert(stat->mtime / 1000).c_str());
	fprintf(stderr, "version = %ld\n", stat->version);
	fprintf(stderr, "cversion = %ld\n", stat->cversion);
	fprintf(stderr, "aversion = %ld\n", stat->aversion);
	fprintf(stderr, "ephemeralOwner = 0x%llx\n", stat->ephemeralOwner);
	fprintf(stderr, "dataLength = %ld\n", stat->dataLength);
	fprintf(stderr, "numChildren = %ld\n", stat->numChildren);
	fprintf(stderr, "pzxid = 0x%llx\n", stat->pzxid);
}

void cat_String_vector(String_vector&  vec)
{
	for (int32_t i = 0; i < vec.count; ++i)
	{
		fprintf(stderr, "%s\n", (vec.data)[i]);
	}
}

int main(int argc, char* argv[])
{
	//zhandle_t* zk = zookeeper_init("192.168.1.234:2181,192.168.1.234:2182,192.168.1.234:2183", watcher, 10000, &myid, NULL, 0);
	zk = zookeeper_init("192.168.1.234:2181,192.168.1.234:2182,192.168.1.234:2183", watcher, 10000, &myid, NULL, 0);
	if (!zk)
	{
		std::cout << "NULL zk" << std::endl;
		exit(0);
	}


	// 1,zoo_create call zoo_acreate in body
	//char buf[1024] = { 0 };
    //if (ZOK != zoo_create(zk, "/zoo", "shanghai", 8, &ZOO_OPEN_ACL_UNSAFE, ZOO_SEQUENCE, buf, 1024))
    //{
    //    std::cout << "create path failed" << std::endl;
	//    zookeeper_close(zk);
    //    exit(0);
    //}
    //fprintf(stderr, "zoo_create: buffer[%s]\n", buf);

	// 2,zoo_acreate
	//if (ZOK != zoo_acreate(zk, "/zoo", "shanghai", 8, &ZOO_OPEN_ACL_UNSAFE, ZOO_SEQUENCE, string_completion, "serialization"))
	//{
    //    std::cout << "zoo_acreate path failed" << std::endl;
	//    zookeeper_close(zk);
    //    exit(0);
	//}
	//sleep(5);

	// 3,zoo_exists
    //int64_t czxid;
    //int64_t mzxid;
    //int64_t ctime; // millsecond
    //int64_t mtime; // millsecond
    //int32_t version;
    //int32_t cversion;
    //int32_t aversion;
    //int64_t ephemeralOwner;
    //int32_t dataLength;
    //int32_t numChildren;
    //int64_t pzxid;

	//struct Stat  st;
	//if (ZOK != zoo_exists(zk, "/zoo0000000005", 1, &st))
	//{
	//	fprintf(stderr, "node not exist!!!\n");
	//  zookeeper_close(zk);
	//	exit(0);
	//}
	//fprintf(stderr, "czxid = 0x%llx\n", st.czxid);
	//fprintf(stderr, "mzxid = 0x%llx\n", st.mzxid);
	//fprintf(stderr, "ctime = %s\n", convert(st.ctime / 1000).c_str());
	//fprintf(stderr, "mtime = %s\n", convert(st.mtime / 1000).c_str());
	//fprintf(stderr, "version = %ld\n", st.version);
	//fprintf(stderr, "cversion = %ld\n", st.cversion);
	//fprintf(stderr, "aversion = %ld\n", st.aversion);
	//fprintf(stderr, "ephemeralOwner = 0x%llx\n", st.ephemeralOwner);
	//fprintf(stderr, "dataLength = %ld\n", st.dataLength);
	//fprintf(stderr, "numChildren = %ld\n", st.numChildren);
	//fprintf(stderr, "pzxid = 0x%llx\n", st.pzxid);
	//asynchorinize
	//if (ZOK != zoo_aexists(zk, "/zoo0000000005", 1, stat_completion, "asynchronize exists"))
	//{
	//	fprintf(stderr, "not exists node\n");
	//	zookeeper_close(zk);
	//	exit(0);
	//}
	//

	//4, get
	struct Stat  gst;
	char  rBuf[1024] = { 0 };
	int len = 1024;
	if (ZOK != zoo_get(zk, "/zoo0000000005", 1, rBuf, &len, &gst))
	{
		fprintf(stderr, "get node value failed!!!\n");
		zookeeper_close(zk);
		exit(0);
	}
	//fprintf(stderr, "rBuf: %s\n", rBuf);
	//fprintf(stderr, "czxid = 0x%llx\n", gst.czxid);
	//fprintf(stderr, "mzxid = 0x%llx\n", gst.mzxid);
	//fprintf(stderr, "ctime = %s\n", convert(gst.ctime / 1000).c_str());
	//fprintf(stderr, "mtime = %s\n", convert(gst.mtime / 1000).c_str());
	//fprintf(stderr, "version = %ld\n", gst.version);
	//fprintf(stderr, "cversion = %ld\n", gst.cversion);
	//fprintf(stderr, "aversion = %ld\n", gst.aversion);
	//fprintf(stderr, "ephemeralOwner = 0x%llx\n", gst.ephemeralOwner);
	//fprintf(stderr, "dataLength = %ld\n", gst.dataLength);
	//fprintf(stderr, "numChildren = %ld\n", gst.numChildren);
	//fprintf(stderr, "pzxid = 0x%llx\n", gst.pzxid);
	//asynchorinize
	//if (ZOK != zoo_aget(zk, "/zoo0000000005", 1, data_completion, "asynchronize get"))
	//{
	//	fprintf(stderr, "asynchronize get node data failed\n");
	//	zookeeper_close(zk);
	//	exit(0);
	//}

	//5,set
	//char wBuf[1024] = { 0 };
	//strncpy(wBuf, "hello ellison!!!", 16);
	//if (ZOK != zoo_set(zk, "/zoo0000000005", wBuf, 1024, gst.version))
	//{
	//	fprintf(stderr, "set node value failed!!!\n");
	//	zookeeper_close(zk);
	//	exit(0);
	//}

	// get child node
	//String_vector cRes;
	//if (ZOK != zoo_get_children(zk, "/zk_test", 1,  &cRes))
	//{
	//	fprintf(stderr, "set node value failed!!!\n");
	//	zookeeper_close(zk);
	//	exit(0);
	//}
	//cat_String_vector(cRes);

	//6 create ephemeral node
	char buf[1024] = { 0 };
    if (ZOK != zoo_create(zk, "/zk_test/zk_child", "shanghai", 8, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, buf, 1024))
    {
        std::cout << "create path failed" << std::endl;
	    zookeeper_close(zk);
        exit(0);
    }
    fprintf(stderr, "zoo_create: buffer[%s]\n", buf);

    std::cout << "+++++++++++++++++++++++++++++++++++++test zookeeper client success++++++++++++++++++++++++++++++++\n" << std::endl;

	while (1)
	{
		sleep(10);
	}
    zookeeper_close(zk);
	return 0;
}

