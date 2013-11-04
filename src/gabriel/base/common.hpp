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
    }

    ~Guard_Ref()
    {
        m_val = out_val;        
    }

    T &m_val;
};
}
}

#endif
