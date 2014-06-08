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
 *   @date: 2014-01-24 11:53:33                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__ORDINARY_SERVER
#define GABRIEL__BASE__ORDINARY_SERVER

#include "gabriel/base/server.hpp"

namespace gabriel {
namespace base {

class Ordinary_Server : public Server
{
public:
    Ordinary_Server();
    virtual ~Ordinary_Server();

protected:
    gabriel::base::Server_Connection m_center_connection;
    ACE_INET_Addr m_supercenter_addr;
    
private:
    virtual void on_connection_shutdown(gabriel::base::Server_Connection *server_connection);
    virtual void do_main_server_connection();
    virtual void do_reconnect();
    virtual int32 init_hook();
    virtual void register_msg_handler();
    virtual void handle_connection_msg(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size);
    void center_addr_rsp(gabriel::base::Server_Connection *server_connection, void *data, uint32 size);
    virtual void on_connection_shutdown_ordinary(gabriel::base::Server_Connection *server_connection);
    virtual void do_main_server_connection_ordinary();
    virtual void reconnect_ordinary();
    virtual int32 init_hook_ordinary() = 0;    
    virtual void register_msg_handler_ordinary();
    virtual void handle_connection_msg_ordinary(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size) = 0;
    void register_req();    
    gabriel::base::Server_Connection m_supercenter_connection;
    gabriel::base::Message_Handler<Ordinary_Server, gabriel::base::Server_Connection> m_supercenter_msg_handler;
};
    
}
}
        
#endif










