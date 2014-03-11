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
 *   @date: 2014-03-11 22:19:35                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__LOG
#define GABRIEL__BASE__LOG

#include <string>
#include <fstream>
#include "ace/Log_Msg_Callback.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Log_Record.h"
#include "ace/Log_Msg.h"
#include "gabriel/base/define.hpp"

namespace gabriel {
namespace base {

class Gabriel_Log_Callback : public ACE_Log_Msg_Callback
{
    friend class Gabriel_Log_Msg;
    
private:
    Gabriel_Log_Callback();
    virtual ~Gabriel_Log_Callback();
    virtual void log (ACE_Log_Record &log_record);
    static std::string to_string(ACE_Log_Priority priority);
    ACE_Recursive_Thread_Mutex m_lock;
    std::string m_log_path;
    int32 m_hour;    
    fstream m_file;    
};
    
class Gabriel_Log_Msg : public ACE_Log_Msg
{
public:
    static Gabriel_Log_Callback* log_callback();
    void init(std::string program_name, std::string log_path);
};

#define LOG_DEBUG(...) \
    do { \
    int const __ace_error = ACE_Log_Msg::last_error_adapter (); \
    Gabriel_Log_Msg *ace___ = Gabriel_Log_Msg::instance (); \
    ace___->conditional_set (__FILE__, __LINE__, 0, __ace_error); \
    ace___->log(LM_DEBUG, __VA_ARGS__); \
  } while (0)

}
}

#endif
