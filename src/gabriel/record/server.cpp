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
 *   @date: 2014-01-09 12:48:05                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include "ace/Dev_Poll_Reactor.h"
#include "gabriel/record/server.hpp"
#include "gabriel/protocol/server/msg_type.pb.h"
#include "gabriel/protocol/server/default.pb.h"

using namespace std;

namespace gabriel {
namespace record {

Server::Server()
{
    type(gabriel::base::RECORD_SERVER);
}

Server::~Server()
{
}

void Server::on_connection_shutdown(gabriel::base::Client_Connection *client_connection)
{
    base::Server::on_connection_shutdown(client_connection);
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(100, true), true), true);
    m_center_connection.reactor(ACE_Reactor::instance());    
}

bool Server::init_hook()
{
    zone_id(1);
    m_supercenter_addr.set(20001);

    return Super::init_hook();
}

void Server::update_hook()
{
}

void Server::register_msg_handler()
{
    using namespace gabriel::protocol::server;
    Super::register_msg_handler();
    m_center_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, this, &Server::register_rsp_from);
}

void Server::handle_connection_msg(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    m_client_msg_handler.handle_message(msg_type, msg_id, client_connection, data, size);
}
    
void Server::register_rsp_from(gabriel::base::Server_Connection *server_connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;    
    PARSE_MSG(Register_Ordinary_Rsp, msg);

    if(id() > 0)
    {
        return;
    }
    
    if(msg.info_size() != 1)
    {
        state(gabriel::base::SERVER_STATE::SHUTDOWN);
        cout << "error: information of this server received from center server is wrong" << endl;
        
        return;
    }
    
    const gabriel::protocol::server::Server_Info &info = msg.info(0);
    id(info.server_id());
    
    if(m_acceptor.open(ACE_INET_Addr(info.port(), info.inner_addr().c_str()), ACE_Reactor::instance()) < 0)
    {
        state(gabriel::base::SERVER_STATE::SHUTDOWN);
        cout << "error: start record server failed" << endl;
            
        return;
    }

    cout << "start record server ok" << endl;
    set_proc_name_and_log_dir("gabriel_record_server___%u___%u", zone_id(), id());
}
    
bool Server::handle_connection_msg(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    if(Super::handle_connection_msg(server_connection, msg_type, msg_id, data, size))
    {
        return true;
    }
    
    if(server_connection == &m_center_connection)
    {
        m_center_msg_handler.handle_message(msg_type, msg_id, server_connection, data, size);
    }

    return true;
}

void Server::fini_hook()
{
}

}
}

#include "gabriel/main.cpp"
