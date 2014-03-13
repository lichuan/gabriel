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
#include "ace/ACE.h"
#include "ace/OS_NS_unistd.h"
#include "gabriel/base/log.hpp"

namespace gabriel {
namespace base {

Log_Callback::Log_Callback()
{
    m_hour = -1;
}

Log_Callback::~Log_Callback()
{
    m_file.close();    
}

std::string Log_Callback::to_string(ACE_Log_Priority priority)
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

void Log_Callback::log(ACE_Log_Record &log_record)
{
    using namespace std;
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_lock);
    ACE_Date_Time now;
    ostringstream ost;
    ost << now.year() << "-" << setw(2) << setfill('0') << now.month() << "-" << setw(2) << setfill('0') << now.day();
    std::string day_path = m_log_path + ost.str();
    ost << " " << setw(2) << setfill('0') << now.hour() << ":" << setw(2) << setfill('0') << now.minute() << ":" << setw(2) << setfill('0') << now.second() << " ";
    std::string time_prefix = ost.str();    
    bool file_changed = false;
    
    if(m_day_path != day_path)
    {
        if(ACE_OS::access(day_path.c_str(), F_OK) != 0)
        {
            ACE_OS::mkdir(day_path.c_str());
        }

        m_day_path = day_path;
        file_changed = true;
    }

    if(m_hour != now.hour())
    {
        file_changed = true;
    }

    ost.str("");
    ost << setw(2) << setfill('0') << now.hour();    
    std::string hour_path = m_day_path + ACE_DIRECTORY_SEPARATOR_STR + ost.str();
    
    if(file_changed)
    {
        m_file.close();
        m_file.open(hour_path, fstream::app);
    }
    
    m_file << to_string(static_cast<ACE_Log_Priority>(log_record.type())) << time_prefix << log_record.msg_data();
}

void Log_Msg::init(std::string log_path)
{
    open("server", ACE_Log_Msg::MSG_CALLBACK);
    Log_Callback *callback = log_callback();
    callback->m_log_path = log_path;
    msg_callback(callback);

    if(ACE_OS::access(log_path.c_str(), F_OK) != 0)
    {
        ACE_OS::mkdir(log_path.c_str());
    }
}

Log_Callback* Log_Msg::log_callback()
{
    static Log_Callback obj;
    
    return &obj;    
}

}
}
