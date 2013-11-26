#ifndef GABRIEL__BASE__THREAD
#define GABRIEL__BASE__THREAD

#include "ace/Task.h"
#include "gabriel/base/define.hpp"

namespace gabriel {
namespace base {

template<typename T>
class Executor : public ACE_Task_Base
{
public:
    Executor()
    {
        m_object = NULL;
        m_num_thread = 0;
    }
    
    void execute()
    {
        activate (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, m_num_thread);
    }
    
    int32 svc()
    {
        (m_object->*m_executor)();

        return 0;
    }
    
    void set_execute_info(T *object, void (T::*executor)(), int32 num_thread)
    {
        m_object = object;
        m_executor = executor;
        m_num_thread = num_thread;
    }
    
private:
    T *m_object;
    void (T::*m_executor)();
    int32 m_num_thread;    
};

template<typename T, int32 MAX_EXECUTOR = 10>
class Thread
{
public:
    Thread()
    {
        m_cur_executor_idx = 0;
    }
    
    void execute()
    {
        for(int32 i = 0; i != m_cur_executor_idx; ++i)
        {
            m_executor_list[i].execute();        
        }
    }

    void wait()
    {
        for(int32 i = 0; i != m_cur_executor_idx; ++i)
        {
            m_executor_list[i].wait();
        }
    }
    
    void add_executor(T *object, void (T::*executor)(), int32 num_thread = 1)
    {
        if(m_cur_executor_idx >= MAX_EXECUTOR)
        {
            return;
        }
    
        m_executor_list[m_cur_executor_idx++].set_execute_info(object, executor, num_thread);
    }

private:
    Executor<T> m_executor_list[MAX_EXECUTOR];
    int32 m_cur_executor_idx;    
};

}
}

#endif
