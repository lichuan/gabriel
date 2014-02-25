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
 *   @date: 2014-01-09 12:40:31                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__LOGIN__SERVER
#define GABRIEL__LOGIN__SERVER

#include "gabriel/base/ordinary_server.hpp"
#include "gabriel/base/message_handler.hpp"

namespace gabriel {
namespace login {

class Server : public gabriel::base::Ordinary_Server
{
public:
    Server();
    virtual ~Server();
    
private:
    virtual void register_msg_handler_ordinary();
    virtual void on_connection_shutdown(gabriel::base::Client_Connection *client_connection);
    virtual void on_connection_shutdown_ordinary(gabriel::base::Server_Connection *server_connection);
    virtual bool verify_connection(gabriel::base::Client_Connection *client_connection);
    virtual void do_main_server_connection_ordinary();
    virtual void update_hook();
    virtual void fini_hook();
    virtual void handle_connection_msg(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size);
    virtual void handle_connection_msg_ordinary(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size);
    virtual void init_reactor();
    virtual int32 init_hook_ordinary();    
    virtual void reconnect_ordinary();    
    void register_rsp(gabriel::base::Server_Connection *server_connection, void *data, uint32 size);
    gabriel::base::Server_Connection m_record_connection;
    gabriel::base::Message_Handler<Server, gabriel::base::Server_Connection> m_center_msg_handler;
    gabriel::base::Message_Handler<Server, gabriel::base::Server_Connection> m_record_msg_handler;
    gabriel::base::Message_Handler<Server, gabriel::base::Client_Connection> m_client_msg_handler;
};
    
}
}

typedef ACE_Singleton<gabriel::login::Server, ACE_Null_Mutex> SERVER;

#endif
