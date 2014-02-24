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
 *   @date: 2014-02-23 20:49:14                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__TIMER
#define GABRIEL__BASE__TIMER

#include <map>
#include "ace/Singleton.h"
#include "ace/Event_Handler.h"
#include "ace/Timer_Heap.h"
#include "gabriel/base/define.hpp"

namespace gabriel {
namespace base {

class Timer_Handler : public ACE_Event_Handler
{
public:
    Timer_Handler(std::function<void()> &call);
    virtual int handle_timeout (const ACE_Time_Value &current_time, const void *act = 0);
    void timer_id(int32 timer_id);
    
private:
    std::function<void()> &m_call;
    int32 m_timer_id;
};

typedef ACE_Singleton<ACE_Timer_Heap, ACE_Null_Mutex> TIMER_MGR;

class Timer_Capability
{
public:
    Timer_Capability();
    ~Timer_Capability();
    
    int32 schedule_timer(std::function<void()> &call, uint32 interval, uint32 delay = 0);
    void cancel_timer(int32 timer_id);
    
private:
    std::map<int32, Timer_Handler*> m_timer_handlers;
};

}
}
        
#endif
