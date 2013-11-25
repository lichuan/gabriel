#include "gabriel/base/thread.hpp"

namespace gabriel {
namespace base {

template<typename T>
Thread_Func_Runner<T>::Thread_Func_Runner()
{
    m_object = NULL;
    m_num_thread = 0;    
}

template<typename T>
void Thread_Func_Runner<T>::run()
{
    activate (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, m_num_thread);
}

template<typename T>
int32 Thread_Func_Runner<T>::svc()
{
    m_object->*m_thread_func();    
}

template<typename T>
void Thread_Func_Runner<T>::set_func_info(T *object, void (T::*thread_func)(), int32 num_thread)
{
    m_object = object;
    m_thread_func = thread_func;
    m_num_thread = num_thread;    
}
    
template<typename T, int32 MAX_FUNC>
Thread_Func_List_Runner<T, MAX_FUNC>::Thread_Func_List_Runner()
{
    m_cur_func_idx = 0;    
}

template<typename T, int32 MAX_FUNC>
void Thread_Func_List_Runner<T, MAX_FUNC>::register_thread_func(T *object, void (T::*thread_func)(), int32 num_thread)
{
    m_func_list[m_cur_func_idx++].set_func_info(object, thread_func, num_thread);    
}
    
}
}
