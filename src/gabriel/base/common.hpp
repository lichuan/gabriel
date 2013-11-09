#ifndef GABRIEL_BASE_COMMON
#define GABRIEL_BASE_COMMON

#include "gabriel/base/define.hpp"

namespace gabriel {
namespace base {
    
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
class Guard_Scope_Val
{
public:
    Guard_Scope_Val(T &val, T in_val, T out_val) : m_val(val)
    {
        m_val = in_val;
        m_out_val = out_val;        
    }

    ~Guard_Scope_Val()
    {
        m_val = m_out_val;        
    }

    T &m_val;
    T m_out_val;    
};
}
}

#endif
