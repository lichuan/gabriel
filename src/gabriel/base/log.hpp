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
#include "ace/Log_Msg_Callback.h"
#include "ace/Null_Mutex.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Log_Record.h"
#include "ace/Log_Msg.h"
#include "ace/Singleton.h"
#include "gabriel/base/define.hpp"

namespace gabriel {
namespace base {

class Log_Callback : public ACE_Log_Msg_Callback
{
public:
    void init(std::string log_path);
    Log_Callback();
    virtual ~Log_Callback();

private:
    virtual void log(ACE_Log_Record &log_record);
    static std::string to_string(ACE_Log_Priority priority);
    std::string m_log_path;
    ACE_Recursive_Thread_Mutex m_lock;
};
    
typedef ACE_Singleton<Log_Callback, ACE_Null_Mutex> LOG_MSG;
    
#define LOG_DEBUG(...) \
    do { \
    int const __ace_error = ACE_Log_Msg::last_error_adapter (); \
    ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
    ace___->conditional_set (__FILE__, __LINE__, 0, __ace_error); \
    ace___->log(LM_DEBUG, __VA_ARGS__); \
  } while (0)

#define LOG_INFO(...) \
    do { \
    int const __ace_error = ACE_Log_Msg::last_error_adapter (); \
    ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
    ace___->conditional_set (__FILE__, __LINE__, 0, __ace_error); \
    ace___->log(LM_INFO, __VA_ARGS__); \
  } while (0)

#define LOG_ERROR(...) \
    do { \
    int const __ace_error = ACE_Log_Msg::last_error_adapter (); \
    ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
    ace___->conditional_set (__FILE__, __LINE__, -1, __ace_error); \
    ace___->log(LM_ERROR, __VA_ARGS__); \
  } while (0)

#define LOG_FATAL(...) \
    do { \
    int const __ace_error = ACE_Log_Msg::last_error_adapter (); \
    ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
    ace___->conditional_set (__FILE__, __LINE__, -1, __ace_error); \
    ace___->log(LM_CRITICAL, __VA_ARGS__); \
  } while (0)
    
}
}

#endif
