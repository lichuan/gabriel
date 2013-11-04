#ifndef GABRIEL_BASE_COMMON
#define GABRIEL_BASE_COMMON

#include "gabriel/base/define.hpp"

namespace gabriel {
namespace base {
    
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
