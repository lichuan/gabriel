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
 *   @date: 2014-01-12 13:55:43                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__MESSAGE_HANDLER
#define GABRIEL__BASE__MESSAGE_HANDLER

#include <map>
#include "gabriel/base/common.hpp"

namespace gabriel {
namespace base{

template<typename T, typename Connection_Type>
struct Message_Hanlder_Info
{
    T *m_obj;
    void (T::*m_func)(Connection_Type *connection, void *data, uint32 size);
};

template<typename T, typename Connection_Type>
class Message_Handler
{
public:
    void register_handler(uint32 msg_type, uint32 msg_id, T *obj, void (T::*handler)(Connection_Type *connection, void *data, uint32 size))
    {
        Message_Hanlder_Info<T, Connection_Type> info;
        info.m_obj = obj;
        info.m_func = handler;
        m_handlers[msg_type][msg_id] = info;
    }
    
    void handle_message(uint32 msg_type, uint32 msg_id, Connection_Type *connection, void *data, uint32 size)
    {
        typename Handlers::iterator iter = m_handlers.find(msg_type);

        if(iter == m_handlers.end())
        {
            return;
        }

        Msg_Id_Map &msg_id_map = iter->second;
        typename Msg_Id_Map::iterator iter_func = msg_id_map.find(msg_id);

        if(iter_func == msg_id_map.end())
        {
            return;
        }

        Message_Hanlder_Info<T, Connection_Type> &info = iter_func->second;
        (info.m_obj->*info.m_func)(connection, data,size);
    }
    
private:
    typedef std::map<uint32, std::map<uint32, Message_Hanlder_Info<T, Connection_Type>>> Handlers;
    typedef std::map<uint32, Message_Hanlder_Info<T, Connection_Type>> Msg_Id_Map;
    Handlers m_handlers;
};

}
}

#endif
