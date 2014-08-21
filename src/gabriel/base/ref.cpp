/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      _____       ___   _____   _____    _   _____   _               *
 *     /  ___|     /   | |  _  \ |  _  \  | | | ____| | |              *
 *     | |        / /| | | |_| | | |_| |  | | | |__   | |              *
 *     | |  _    / / | | |  _  { |  _  /  | | |  __|  | |              *
 *     | |_| |  / /  | | | |_| | | | \ \  | | | |___  | |___           *
 *     \_____/ /_/   |_| |_____/ |_|  \_\ |_| |_____| |_____|          *
 *                                                                     *
 *     gabriel is an angel from the Holy Bible, this engine is         *
 *   named gabriel, standing for bring good news to people. the        *
 *   goal of gabriel engine is to help people to develop server        *
 *   for online games, welcome you to join in.                         *
 *                                                                     *
 *   @author: lichuan                                                  *
 *   @qq: 308831759                                                    *
 *   @email: 308831759@qq.com                                          *
 *   @github: https://github.com/lichuan/gabriel                       *
 *   @date: 2014-08-21 13:41:47                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "gabriel/base/ref.hpp"

namespace gabriel {
namespace base {

template<typename LOCK>
Ref<LOCK>::Ref() : ACE_Refcountable_T<LOCK>(1)
{
}

template<typename LOCK>
Ref<LOCK>::~Ref()
{
}

template<typename LOCK>
void Ref<LOCK>::retain()
{
    Super::increment();
}

template<typename LOCK>    
void Ref<LOCK>::release()
{
    if(Super::decrement() == 0)
    {
        destroy_this_ref();
    }    
}

template<typename LOCK>
void Ref<LOCK>::destroy_this_ref()
{
    delete this;
}

template class Ref<ACE_SYNCH_MUTEX>;
template class Ref<ACE_SYNCH_NULL_MUTEX>;
    
}
}
