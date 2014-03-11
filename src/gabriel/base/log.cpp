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
 *   @date: 2014-03-11 22:56:27                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <sstream>
#include <iomanip>
#include "ace/Date_Time.h"
#include "ace/Guard_T.h"
#include "gabriel/base/log.hpp"

namespace gabriel {
namespace base {

Gabriel_Log_Callback::Gabriel_Log_Callback()
{
    m_hour = -1;
}

Gabriel_Log_Callback::~Gabriel_Log_Callback()
{
}

std::string Gabriel_Log_Callback::to_string(ACE_Log_Priority priority)
{
    if(priority == LM_DEBUG)
    {
        return "[debug] ";
    }

    if(priority == LM_INFO)
    {
        return "[info] ";
    }

    if(priority == LM_ERROR)
    {
        return "[error] ";
    }

    if(priority == LM_CRITICAL)
    {
        return "[fatal] ";
    }

    return "[unknown] ";    
}

void Gabriel_Log_Callback::log(ACE_Log_Record &log_record)
{
    using std::setw;
    using std::setfill;
    using std::ostringstream;    
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_lock);    
    ACE_Date_Time now;
    ostringstream ost;
    ost << m_log_path << "/";
    
    if(m_hour != -1)
    {
        if(m_hour == now.hour())
        {
            m_file << to_string(static_cast<ACE_Log_Priority>(log_record.type())) << log_record.msg_data();

            return;
        }
        
        if(m_hour < 23)
        {
            m_hour = now.hour();
            ost << now.year() << "-" << setw(2) << setfill('0') << now.month() << setw(2) << setfill('0') << now.day();            
        }        
    }
}

void Gabriel_Log_Msg::init(std::string program_name, std::string log_path)
{
    open(program_name.c_str(), ACE_Log_Msg::MSG_CALLBACK);
    Gabriel_Log_Callback *callback = log_callback();
    callback->m_log_path = log_path;
    msg_callback(callback);
}

Gabriel_Log_Callback* Gabriel_Log_Msg::log_callback()
{
    static Gabriel_Log_Callback obj;
    
    return &obj;    
}

}
}



