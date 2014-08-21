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
 *   @date: 2014-06-21 10:27:53                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include "ace/Dev_Poll_Reactor.h"
#include "gabriel/superrecord/server.hpp"
#include "gabriel/protocol/server/msg_type.pb.h"
#include "gabriel/protocol/server/default.pb.h"

using namespace std;

namespace gabriel {
namespace superrecord {

Server::Server()
{
    type(gabriel::base::SUPERRECORD_SERVER);
}

Server::~Server()
{
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return false;    
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(100, true), true), true);
    m_supercenter_connection.reactor(ACE_Reactor::instance());
}

bool Server::init_hook()
{
    //m_supercenter_addr.set(20001);
    set_proc_name_and_log_dir("gabriel_superrecord_server");
    gabriel::base::Server_Connection *tmp = &m_supercenter_connection;
    
    if(m_connector.connect(tmp, ACE_INET_Addr(20001, "106.186.20.182")) < 0)
    {
        cout << "error: connect to supercenter server failed" << endl;

        return false;
    }
    
    cout << "connect to supercenter server ok" << endl;
    
    return true;
}
    
void Server::update_hook()
{
}

void Server::register_msg_handler()
{
    using namespace gabriel::protocol::server;
    //m_supercenter_msg_handler.register_handler(DEFAULT_MSG_TYPE, ZONE_INFO_REQ, this, &Server::zone_info_req_from);
}

void Server::load_zone_info()
{
}

void Server::zone_info_req_from(gabriel::base::Server_Connection *client_connection, void *data, uint32 size)
{
    load_zone_info();    
}
    
void Server::handle_connection_msg(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
}

bool Server::handle_connection_msg(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    if(server_connection == &m_supercenter_connection)
    {
        m_supercenter_msg_handler.handle_message(msg_type, msg_id, server_connection, data, size);
    }
    
    return true;
}
    
void Server::fini_hook()
{
}

}
}

#include "gabriel/main.cpp"
