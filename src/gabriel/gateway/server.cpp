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
#include "gabriel/gateway/server.hpp"
#include "gabriel/protocol/server/msg_type.pb.h"
#include "gabriel/protocol/server/default.pb.h"

using namespace std;

namespace gabriel {
namespace gateway {
    
Server::Server()
{
    type(gabriel::base::GATEWAY_SERVER);    
}

Server::~Server()
{
}

void Server::on_connection_shutdown(gabriel::base::Client_Connection *client_connection)
{
    base::Server::on_connection_shutdown(client_connection);
}

bool Server::on_connection_shutdown(gabriel::base::Server_Connection *server_connection)
{
    if(Super::on_connection_shutdown(server_connection))
    {
        return true;
    }
    
    if(server_connection == &m_record_connection)
    {
        cout << "error: disconnected from record server" << endl;
    }
    
    for(auto iter : m_game_connections)
    {
        gabriel::base::Server_Connection *game_connection = iter.second;

        if(game_connection == server_connection)
        {
            cout << "error: disconnected from game server (id=" << iter.first << ")" << endl;

            break;
        }
    }
    
    return true;
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
}

void Server::update_hook()
{
}

void Server::do_reconnect_i()
{
    if(m_record_connection.lost_connection())
    {
        gabriel::base::Server_Connection *tmp = &m_record_connection;
            
        if(m_connector.connect(tmp, m_record_connection.inet_addr()) < 0)
        {
            cout << "error: reconnect to record server failed" << endl;
        }
        else
        {
            cout << "reconnect to record server ok" << endl;
        }
    }

    for(auto iter : m_game_connections)
    {
        gabriel::base::Server_Connection *game_connection = iter.second;
        
        if(game_connection->lost_connection())
        {
            if(m_connector.connect(game_connection, game_connection->inet_addr()) < 0)
            {
                cout << "error: reconnect to game server (id=" << iter.first << ") failed" << endl;
            }
            else
            {
                cout << "reconnect to game server (id=" << iter.first << ") ok" << endl;
            }
        }
    }
}

void Server::register_msg_handler()
{    
    using namespace gabriel::protocol::server;
    Super::register_msg_handler();    
    m_center_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, this, &Server::register_rsp_from);
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(5000, true), true), true);
    m_center_connection.reactor(ACE_Reactor::instance());    
    m_record_connection.reactor(ACE_Reactor::instance());
}

bool Server::init_hook()
{
    zone_id(1);
    m_supercenter_addr.set(20001);

    return Super::init_hook();
}
    
void Server::handle_connection_msg(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    m_client_msg_handler.handle_message(msg_type, msg_id, client_connection, data, size);
}

void Server::do_main_on_server_connection()
{
    Super::do_main_on_server_connection();
    m_record_connection.do_main();
}

void Server::register_rsp_from(gabriel::base::Server_Connection *server_connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;    
    PARSE_MSG(Register_Ordinary_Rsp, msg);
    
    if(id() > 0)
    {
        return;
    }
    
    if(msg.info_size() < 3)
    {
        state(gabriel::base::SERVER_STATE::SHUTDOWN);
        cout << "error: information of this server received from center server is wrong" << endl;
        
        return;
    }
    
    for(uint32 i = 0; i != msg.info_size(); ++i)
    {
        const auto &info = msg.info(i);

        if(info.server_type() == gabriel::base::GATEWAY_SERVER)
        {
            id(info.server_id());
            
            if(m_acceptor.open(ACE_INET_Addr(info.port(), info.outer_addr().c_str()), ACE_Reactor::instance()) < 0)
            {
                cout << "error: start gateway server (id=" << id() << ") failed" << endl;
                state(gabriel::base::SERVER_STATE::SHUTDOWN);
                
                return;
            }

            cout << "start gateway server (id=" << id() << ") ok" << endl;
            set_proc_name_and_log_dir("gabriel_gateway_server___%u___%u", zone_id(), id());
        }
        else if(info.server_type() == gabriel::base::RECORD_SERVER)
        {
            gabriel::base::Server_Connection *tmp = &m_record_connection;
            
            if(m_connector.connect(tmp, ACE_INET_Addr(info.port(), info.inner_addr().c_str())) < 0)
            {
                cout << "error: connect to record server failed" << endl;
                state(gabriel::base::SERVER_STATE::SHUTDOWN);
            }
            else
            {
                cout << "connect to record server ok" << endl;
            }
        }
        else
        {
            m_game_connections[info.server_id()] = new gabriel::base::Server_Connection;
            
            if(m_connector.connect(m_game_connections[info.server_id()], ACE_INET_Addr(info.port(), info.inner_addr().c_str())) < 0)
            {
                cout << "error: connect to game server (id=" << info.server_id() << ") failed" << endl;
                state(gabriel::base::SERVER_STATE::SHUTDOWN);
            }
            else
            {
                cout << "connect to game server (id=" << info.server_id() << ") ok" << endl;
            }
        }
    }
}

void Server::fini_hook()
{
    for(auto iter : m_game_connections)
    {
        delete iter.second;        
    }

    m_game_connections.clear();
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
    else if(server_connection == &m_record_connection)
    {
        m_record_msg_handler.handle_message(msg_type, msg_id, server_connection, data, size);
    }

    return true;
}

}
}

#include "gabriel/main.cpp"
