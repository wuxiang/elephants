#include "TinyObject.h"

/*********************TinyCount*************************/
TinyCount::TinyCount(): seed(0)
{

}

long  TinyCount::add_atom()
{
    boost::lock_guard<boost::mutex>  lock(mtx);
    ++seed;
    return seed;
}

long  TinyCount::values()
{
    boost::lock_guard<boost::mutex>  lock(mtx);
    return seed;
}

/*********************run flag counter*******************/
RunFlagCount::RunFlagCount(): total(0), success(0), failed(0)
{
}

int RunFlagCount::tAdd()
{
    boost::lock_guard<boost::mutex> lock(mtx);
    return ++total;
}

int RunFlagCount::sAdd()
{
    boost::lock_guard<boost::mutex> lock(mtx);
    return ++success;
}

int RunFlagCount::fAdd()
{
    boost::lock_guard<boost::mutex> lock(mtx);
    return ++failed;
}

int RunFlagCount::tValue()
{
    boost::lock_guard<boost::mutex> lock(mtx);
    return total;
}

int RunFlagCount::sValue()
{
    boost::lock_guard<boost::mutex> lock(mtx);
    return  success;
}

int RunFlagCount::fValue()
{
    boost::lock_guard<boost::mutex>  lock(mtx);
    return failed;
}


/********************StrTime******************************/
std::string  StrTime::tStr()
{
    std::time_t  ti;
    time(&ti);
    tm timeinfo = {0};
#if defined(WIN32)
    if(localtime_s(&timeinfo, &ti)){
#else
    if(NULL == localtime_r(&ti, &timeinfo)){
#endif
        return std::string();
    }
    char ctime[100];
    sprintf(ctime,"%04d%02d%02d%02d%02d",
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min
        );
    return std::string(ctime);

}

/******************print message*********************/
//void PrintMessage::printReqMessage(const msgbus::Message& msg)
//{
//    DA_ERRLOG("");
//    DA_ERRLOG("[Type](%s)",msg.GetType().c_str());
//    DA_ERRLOG("[Subject](%s)",msg.GetSubject().c_str());
//    DA_ERRLOG("[MessageID](%s)",msg.GetMessageID().c_str());
//    DA_ERRLOG("[ReplyTo](%s)",msg.GetReplyTo().c_str());
//    print_map(msg.MapValue(),0);
//}
//
//void PrintMessage::print_map(const msgbus::Message::Map &map, int depth)
//{
//    std::string blank;
//    for(int i = 0; i < depth; i++){
//        blank += "--";
//    }
//    std::string key;
//    int type;
//    for (int i = 0; i < map.fieldCount(); i++) {
//        map.getFieldInfo(i,key,type);
//        if(msgbus::Message::FIELD_BOOL == type){
//            bool ret;
//            map.getBool(key, ret);
//            DA_ERRLOG("%s<BOOL  >:[%s](%s)",blank.c_str(),key.c_str(), ret?"true":"false");
//        }
//        else if(msgbus::Message::FIELD_UINT8 == type){
//            msgbus::uint8_t ret;
//            map.getUint8(key, ret);
//            DA_ERRLOG("%s<UINT8 >:[%s](%d)",blank.c_str(),key.c_str(), ret);
//        }
//        else if(msgbus::Message::FIELD_UINT16 == type){
//            msgbus::uint16_t ret;
//            map.getUint16(key, ret);
//            DA_ERRLOG("%s<UINT16>:[%s](%d)",blank.c_str(),key.c_str(), ret);
//        }
//        else if(msgbus::Message::FIELD_UINT32 == type){
//            msgbus::uint32_t ret;
//            map.getUint32(key, ret);
//            DA_ERRLOG("%s<UINT32>:[%s](%d)",blank.c_str(),key.c_str(), ret);
//        }
//        else if(msgbus::Message::FIELD_UINT64 == type){
//            msgbus::uint64_t ret;
//            map.getUint64(key, ret);
//            DA_ERRLOG("%s<UINT64>:[%s](%lld)",blank.c_str(),key.c_str(), ret);
//        }
//        else if(msgbus::Message::FIELD_INT8 == type){
//            msgbus::int8_t ret;
//            map.getInt8(key, ret);
//            DA_ERRLOG("%s<INT8  >:[%s](%d)",blank.c_str(),key.c_str(), ret);
//        }
//        else if(msgbus::Message::FIELD_INT16 == type){
//            msgbus::int16_t ret;
//            map.getInt16(key, ret);
//            DA_ERRLOG("%s<INT16 >:[%s](%d)",blank.c_str(),key.c_str(), ret);
//        }
//        else if(msgbus::Message::FIELD_INT32 == type){
//            msgbus::int32_t ret;
//            map.getInt32(key, ret);
//            DA_ERRLOG("%s<INT32 >:[%s](%d)",blank.c_str(),key.c_str(), ret);
//        }
//        else if(msgbus::Message::FIELD_INT64 == type){
//            msgbus::int64_t ret;
//            map.getInt64(key, ret);
//            DA_ERRLOG("%s<INT64 >:[%s](%lld)",blank.c_str(),key.c_str(), ret);
//        }
//        else if(msgbus::Message::FIELD_FLOAT == type){
//            float ret;
//            map.getFloat(key, ret);
//            DA_ERRLOG("%s<FLOAT >:[%s](%f)",blank.c_str(),key.c_str(), ret);
//        }
//        else if(msgbus::Message::FIELD_DOUBLE == type){
//            double ret;
//            map.getDouble(key, ret);
//            DA_ERRLOG("%s<DOUBLE>:[%s](%f)",blank.c_str(),key.c_str(), ret);
//        }
//        else if(msgbus::Message::FIELD_STRING == type){
//            std::string ret;
//            map.getString(key,ret);
//            DA_ERRLOG("%s<STRING>:[%s](%s)",blank.c_str(),key.c_str(), ret.c_str());
//        }
//        else if(msgbus::Message::FIELD_MESSAGE == type){
//            msgbus::Message::Map ret;
//            map.getMap(key,ret);
//            DA_ERRLOG("%s<MAP   >:[%s]",blank.c_str(),key.c_str());
//            print_map(ret, depth + 1);
//        }
//        else if(msgbus::Message::FIELD_LIST == type){
//            msgbus::Message::List ret;
//            map.getList(key,ret);
//            DA_ERRLOG("%s<LIST  >:[%s]",blank.c_str(),key.c_str());
//            int i = 0;
//            for(msgbus::Message::List::const_iterator it = ret.begin(); it != ret.end(); ++it, ++i){
//                DA_ERRLOG("%s[%6d]",blank.c_str(), i);
//                print_map(*it, depth + 1);
//            }
//        }
//        else{
//            DA_ERRLOG("%s<%6d>:[%s]()",blank.c_str(),type,key.c_str());
//        }
//    }
//}


