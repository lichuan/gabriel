/*
 author: lichuan
 qq: 308831759
 email: 308831759@qq.com
 homepage: www.lichuan.me
 github: https://github.com/lichuan/gabriel
 date: 2013-08-23
 desc: a星寻路的一种高效实现
*/

#include "base/astar.hpp"

int32 Point::hash() const
{
    return m_x * 10000 + m_y;
}    
