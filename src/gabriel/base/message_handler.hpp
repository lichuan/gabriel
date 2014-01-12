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

template<typename Connection_Type>
class Message_Handler
{
public:
    void register_handler(uint32 msg_type, uint32 msg_id, void (*handler)(Connection_Type *connection, void *data, uint32 size));
    void handle_message(uint32 msg_type, uint32 msg_id, Connection_Type *connection, void *data, uint32 size);
    
private:
    typedef std::map<uint32, std::map<uint32, void (*)(Connection_Type *connection, void *data, uint32 size)> > Handlers;
    typedef std::map<uint32, void (*)(Connection_Type *connection, void *data, uint32 size)> Msg_Id_Map;
    Handlers m_handlers;
};

}
}

#endif
