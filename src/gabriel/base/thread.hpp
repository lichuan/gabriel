#ifndef GABRIEL__BASE__THREAD
#define GABRIEL__BASE__THREAD

#include "ace/Task.h"
#include "gabriel/base/define.hpp"

namespace gabriel {
namespace base {

template<typename T>
class Thread_Func_Runner : public ACE_Task_Base
{
public:
    Thread_Func_Runner()
    {
        m_object = NULL;
        m_num_thread = 0;
    }
    
    void run()
    {
        activate (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, m_num_thread);
    }
    
    int32 svc()
    {
        (m_object->*m_thread_func)();
        
        return 0;
    }
    
    void set_func_info(T *object, void (T::*thread_func)(), int32 num_thread)
    {
        m_object = object;
        m_thread_func = thread_func;
        m_num_thread = num_thread;
    }
    
private:
    T *m_object;
    void (T::*m_thread_func)();
    int32 m_num_thread;    
};

template<typename T, int32 MAX_FUNC>
class Thread_Func_List_Runner
{
public:
    Thread_Func_List_Runner()
    {
        m_cur_func_idx = 0;
    }
    
    void run_thread_func()
    {
        for(int32 i = 0; i != m_cur_func_idx; ++i)
        {
            m_func_list[i].run();        
        }
    }

    void wait()
    {
        for(int32 i = 0; i != m_cur_func_idx; ++i)
        {
            m_func_list[i].wait();
        }
    }
    
    void register_thread_func(T *object, void (T::*thread_func)(), int32 num_thread)
    {
        if(m_cur_func_idx >= MAX_FUNC)
        {
            return;
        }
    
        m_func_list[m_cur_func_idx++].set_func_info(object, thread_func, num_thread);
    }

private:
    Thread_Func_Runner<T> m_func_list[MAX_FUNC];
    int32 m_cur_func_idx;    
};

}
}

#endif
