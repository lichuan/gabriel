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

#include "gabriel/base/define.hpp"

namespace gabriel {
namespace base {

enum SERVER_TYPE
{
    INVALID_SERVER = 0,
    SUPERCENTER_SERVER = 1,    
    CENTER_SERVER = 2,
    RECORD_SERVER = 3,
    LOGIN_SERVER = 4,
    GAME_SERVER = 5,
    GATEWAY_SERVER = 6,
};

enum CLIENT_TYPE
{
    INVALID_CLIENT = 0,
    CENTER_CLIENT = CENTER_SERVER,
    RECORD_CLIENT = RECORD_SERVER,
    LOGIN_CLIENT = LOGIN_SERVER,
    GAME_CLIENT = GAME_SERVER,
    GATEWAY_CLIENT = GATEWAY_SERVER,
    USER_CLIENT = GATEWAY_CLIENT + 1,
};

struct Point
{
    Point(int32 x, int32 y)
    {
        m_x = x;
        m_y = y;
    }

    Point()
    {
        m_x = 0;
        m_y = 0;
    }

    int32 hash() const
    {
        return m_x * 10000 + m_y;        
    }
    
    bool operator==(const Point &pos) const
    {
        return m_x == pos.m_x && m_y == pos.m_y;
    }    
    
    int32 m_x;
    int32 m_y;
};

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
    
}
}

#endif
