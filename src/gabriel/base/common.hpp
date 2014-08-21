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
 *   @date: 2013-11-29 09:00:01                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__COMMON
#define GABRIEL__BASE__COMMON

#include <string>
#include "gabriel/base/define.hpp"

namespace gabriel {
namespace base {

template<typename T>
class Guard_Scope_Value
{
public:
    Guard_Scope_Value(T &value, T in_value, T out_value) : m_value(value)
    {
        m_value = in_value;
        m_out_value = out_value;
    }
    
    ~Guard_Scope_Value()
    {
        m_value = m_out_value;
    }
            
    T &m_value;
    T m_out_value;    
};

template<typename T = uint32>
class ID_Allocator
{
public:
    ID_Allocator()
    {
        m_id = 1;
    }

    T new_id();    
    
private:
    T m_id;    
};

void sleep_sec(uint32 sec);
void sleep_msec(uint32 msec);
void sleep_usec(uint32 usec);
uint32 random_between(uint32 min, uint32 max);
uint32 random_32();
uint64 random_64();
bool rate_by_percent(uint32 rate);    
bool rate_by_thousand(uint32 rate);
uint64 get_usec_tick();
uint64 get_msec_tick();
uint32 get_sec_tick();    
std::string gen_uuid_str();
uint64 gen_uuid();

}
}

#endif
