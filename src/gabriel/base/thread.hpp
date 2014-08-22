/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      _____       ___   _____   _____    _   _____   _               *
 *     /  ___|     /   | |  _  \ |  _  \  | | | ____| | |              *
 *     | |        / /| | | |_| | | |_| |  | | | |__   | |              *
 *     | |  _    / / | | |  _  { |  _  /  | | |  __|  | |              *
 *     | |_| |  / /  | | | |_| | | | \ \  | | | |___  | |___           *
 *     \_____/ /_/   |_| |_____/ |_|  \_\ |_| |_____| |_____|          *
 *                                                                     *
 *     gabriel is an angel from the Holy Bible, this engine is named   *
 *   gabriel, means bringing people good news. the goal of gabriel     *
 *   server engine is to help people to develop various online games,  *
 *   welcome you to join in.                                           *
 *                                                                     *
 *   @author: lichuan                                                  *
 *   @qq: 308831759                                                    *
 *   @email: 308831759@qq.com                                          *
 *   @site: www.lichuan.me                                             *
 *   @github: https://github.com/lichuan/gabriel                       *
 *   @date: 2013-11-29 09:00:42                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__THREAD
#define GABRIEL__BASE__THREAD

#include "ace/Task.h"
#include "gabriel/base/common.hpp"

namespace gabriel {
namespace base {

class Executor : public ACE_Task_Base
{
public:
    Executor()
    {
        m_num_thread = 0;
        m_waited = false;        
    }
    
    void execute()
    {
        activate (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, m_num_thread);
    }
    
    int32 svc()
    {
        m_func();

        return 0;
    }

    int wait()
    {
        if(!m_waited)
        {
            m_waited = true;
            
            return ACE_Task_Base::wait();
        }

        return 0;        
    }
    
    void set_execute_info(std::function<void()> func, int32 num_thread)
    {
        m_func = func;
        m_num_thread = num_thread;
    }
    
private:
    std::function<void()> m_func;
    int32 m_num_thread;
    bool m_waited;    
};

template<int32 MAX_EXECUTOR = 10>
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

    void wait(int32 idx)
    {
        if(idx < 0 || idx >= MAX_EXECUTOR)
        {
            return;
        }

        m_executor_list[idx].wait();
    }
    
    int32 start_executor_instantly(std::function<void()> func, int32 num_thread = 1)
    {
        if(m_cur_executor_idx >= MAX_EXECUTOR)
        {
            return -1;
        }

        m_executor_list[m_cur_executor_idx].set_execute_info(func, num_thread);
        m_executor_list[m_cur_executor_idx].execute();
        
        return m_cur_executor_idx++;
    }
    
    void add_executor(std::function<void()> func, int32 num_thread = 1)
    {
        if(m_cur_executor_idx >= MAX_EXECUTOR)
        {
            return;
        }

        m_executor_list[m_cur_executor_idx++].set_execute_info(func, num_thread);
    }
    
private:
    Executor m_executor_list[MAX_EXECUTOR];
    int32 m_cur_executor_idx;    
};

}
}

#endif
