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
 *   @date: 2014-01-12 14:14:24                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "gabriel/base/message_handler.hpp"
#include "gabriel/base/server_connection.hpp"
#include "gabriel/base/client_connection.hpp"

namespace gabriel {
namespace base {

template<typename Connection_Type>
void Message_Handler<Connection_Type>::register_handler(uint32 msg_type, uint32 msg_id, void (*handler)(Connection_Type *connection, void *data, uint32 size))
{
    m_handlers[msg_type][msg_id] = handler;    
}

template<typename Connection_Type>
void Message_Handler<Connection_Type>::handle_message(uint32 msg_type, uint32 msg_id, Connection_Type *connection, void *data, uint32 size)
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

    (*iter_func->second)(connection, data, size);
}

template class Message_Handler<Client_Connection>;
template class Message_Handler<Server_Connection>;

}
}
