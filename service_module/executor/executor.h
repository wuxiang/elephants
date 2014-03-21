#ifndef EXECUTOR_H_
#define EXECUTOR_H_
#include <boost/thread.hpp>

namespace Elephants
{
    template<class T> void cleanup_function(T* ptr)
    {
        delete ptr;
    }

    class ExecutorBase
    {
    public:
        ExecutorBase();
        virtual ~ExecutorBase();
        virtual void operator()() = 0;

    protected:
        boost::thread::id    m_id;
    };

}

#endif

