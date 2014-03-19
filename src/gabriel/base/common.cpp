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
 *   @date: 2013-11-29 22:59:46                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "ace/OS.h"
#include "ace/UUID.h"
#include "gabriel/base/common.hpp"

namespace gabriel {
namespace base {

template<>
uint32 ID_Allocator<uint32>::new_id()
{
    if(m_id >= MAX_UINT32)
    {
        m_id = 1;

        return MAX_UINT32;
    }
    
    return m_id++;
}

template<>
uint64 ID_Allocator<uint64>::new_id()
{
    if(m_id >= MAX_UINT64)
    {
        m_id = 1;

        return MAX_UINT64;
    }

    return m_id++;
}

void sleep_sec(uint32 sec)
{
    ACE_OS::sleep(ACE_Time_Value(sec, 0));
}

void sleep_msec(uint32 _msec)
{
    uint32 sec = 0;
    uint32 msec = _msec;
    
    if(msec >= ACE_ONE_SECOND_IN_MSECS)
    {
        sec = msec / ACE_ONE_SECOND_IN_MSECS;
        msec = msec % ACE_ONE_SECOND_IN_MSECS;
    }
    
    ACE_OS::sleep(ACE_Time_Value(sec, msec * 1000));
}

void sleep_usec(uint32 _usec)
{
    uint32 sec = 0;
    uint32 usec = _usec;
    
    if(usec >= ACE_ONE_SECOND_IN_USECS)
    {
        sec = usec / ACE_ONE_SECOND_IN_USECS;
        usec = usec % ACE_ONE_SECOND_IN_USECS;
    }

    ACE_OS::sleep(ACE_Time_Value(sec, usec));
}

uint32 random_32()
{
    static std::random_device rd;
    static std::mt19937 mt(rd());

    return mt();    
}

uint64 random_64()
{
    uint64 a = random_32();
    uint64 b = random_32();

    return (a << 32) + b;
}

uint32 random_between(uint32 min, uint32 max)
{
    if(min == max)
    {
        return min;
    }

    if(min > max)
    {
        std::swap(min, max);
    }
    
    const uint32 diff = max - min + 1;

    return min + random_32() % diff;
}

bool rate_by_percent(uint32 rate)
{
    return random_between(1, 100) <= rate;
}

bool rate_by_thousand(uint32 rate)
{
    return random_between(1, 1000) <= rate;
}

std::string gen_uuid()
{
    ACE_Utils::UUID uuid;
    ACE_Utils::UUID_GENERATOR::instance()->generate_UUID(uuid);

    return uuid.to_string()->c_str();
}

}
}
