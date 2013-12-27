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

struct Point
{
    Point(int32 x, int32 y);    
    Point();
    int32 hash() const;    
    bool operator==(const Point &pos) const;    
    int32 m_x;
    int32 m_y;
};

template<typename T>
class Guard_Scope_Value
{
public:
    Guard_Scope_Value(T &value, T in_value, T out_value) : m_value(value)
    {
        m_value = value;
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

template<>
uint32 ID_Allocator<uint32>::new_id()
{
    if(m_id >= MAX_UINT32)
    {
        m_id = 1;
    }

    return m_id++;
}

template<>
uint64 ID_Allocator<uint64>::new_id()
{
    if(m_id >= MAX_UINT64)
    {
        m_id = 1;
    }

    return m_id++;
}

}
}

#endif
