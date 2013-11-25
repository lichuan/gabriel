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
    Thread_Func_Runner();
    void run();    
    int32 svc();
    void set_func_info(T *object, void (T::*thread_func)(), int32 num_thread);

private:
    T *m_object;
    void (T::*m_thread_func)();
    int32 m_num_thread;    
};

template<typename T, int32 MAX_FUNC>
class Thread_Func_List_Runner
{
public:
    Thread_Func_List_Runner();
    void run_thread_func();
    void register_thread_func(T *object, void (T::*thread_func)(), int32 num_thread);
    
private:
    Thread_Func_Runner<T> m_func_list[MAX_FUNC];
    int32 m_cur_func_idx;    
};

}
}

#endif
