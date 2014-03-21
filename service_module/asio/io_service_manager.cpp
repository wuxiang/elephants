#include "io_service_manager.h"

namespace Elephants
{
    bool  io_service_manager::init(const std::size_t  sz)
    {
        if (sz <= 0)
        {
            return false;
        }

        boost::unique_lock<boost::shared_mutex>  lock(mtx4init);
        if (isInited)
        {
            return true;
        }
        pool.reset(new (std::nothrow) io_service_pool(sz));
        isInited = true;
        return true;
    }

    boost::asio::io_service&  io_service_manager::get_io_service()
    {
        {
            boost::shared_lock<boost::shared_mutex> lock(mtx4init);
            if (!isInited)
            {
                throw  std::runtime_error("io_service_manager is not initialized");
            }
        }

        return pool->get_io_service();
    }

    io_service_manager::io_service_manager(): isInited(false)
    {

    }

    io_service_manager::~io_service_manager()
    {
        if (isInited)
        {
            pool->stop();
        }
    }


}


