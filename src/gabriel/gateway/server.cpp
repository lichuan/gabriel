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
#include "yaml-cpp/yaml.h"
#include "ace/Dev_Poll_Reactor.h"
#include "gabriel/gateway/server.hpp"
#include "gabriel/protocol/server/default.pb.h"
#include "gabriel/protocol/server/msg_type.pb.h"

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
}

bool Server::on_connection_shutdown(gabriel::base::Server_Connection *server_connection)
{
    if(Super::on_connection_shutdown(server_connection))
    {
        return true;
    }
    
    for(auto iter : m_game_connections)
    {
        gabriel::base::Server_Connection *game_connection = iter.second;

        if(game_connection == server_connection)
        {
            cout << "error: disconnected from game server (id=" << iter.first << ")" << endl;
            LOG_ERROR("disconnected from game server (id=%u)", iter.first);

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

void Server::do_reconnect()
{
    Super::do_reconnect();
    
    for(auto iter : m_game_connections)
    {
        gabriel::base::Server_Connection *game_connection = iter.second;
        
        if(game_connection->lost_connection())
        {
            if(m_connector.connect(game_connection, game_connection->inet_addr()) < 0)
            {
                cout << "error: reconnect to game server (id=" << iter.first << ") failed" << endl;
                LOG_ERROR("reconnect to game server (id=%u) failed", iter.first);
            }
            else
            {
                cout << "reconnect to game server (id=" << iter.first << ") ok" << endl;
                LOG_INFO("reconnect to game server (id=%u) ok", iter.first);
            }
        }
    }
}
    
void Server::register_msg_handler()
{    
    using namespace gabriel::protocol::server;
    using namespace placeholders;
    Super::register_msg_handler();
    m_server_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, bind(&Server::register_rsp_from, this, _1, _2, _3));
    m_server_msg_handler.register_handler(DEFAULT_MSG_TYPE, SYNC_ACCOUNT, bind(&Server::sync_account, this, _1, _2, _3));
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(10000, true), true), true);
}

bool Server::init_hook()
{
    schedule_timer([=]() {
            for(auto iter = m_accounts.begin(); iter != m_accounts.end();)
            {
                Account_Info &info = iter->second;
                uint32 now_sec = base::get_sec_tick();

                if(now_sec - info.birth_time > 300)
                {
                    m_accounts.erase(iter++);
                }
                else
                {
                    ++iter;
                }
            }
        }, 60000, 300000);

    return Super::init_hook();
}

void Server::do_main_on_server_connection()
{
    Super::do_main_on_server_connection();

    for(auto iter : m_game_connections)
    {
        gabriel::base::Server_Connection *game_connection = iter.second;
        game_connection->do_main();
    }
}

void Server::sync_account(gabriel::base::Connection *connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;
    PARSE_FROM_ARRAY(Sync_Account, msg, data, size);
    Account_Info info;
    info.birth_time = base::get_sec_tick();
    info.key = msg.key();
    m_accounts[msg.account()] = info;
    Sync_Account_Rsp rsp;
    rsp.set_key(info.key);
    rsp.set_port(msg.port());
    rsp.set_addr(msg.addr());
    rsp.set_conn_id_1(msg.conn_id_1());
    rsp.set_conn_id_2(msg.conn_id_2());
    connection->send(DEFAULT_MSG_TYPE, SYNC_ACCOUNT, rsp);
}

void Server::register_rsp_from(gabriel::base::Connection *connection, void *data, uint32 size)
{
    if(id() > 0)
    {
        return;
    }
    
    using namespace gabriel::protocol::server;
    PARSE_FROM_ARRAY(Register_Ordinary_Rsp, msg, data, size);
    
    if(msg.info_size() < 3)
    {
        state(gabriel::base::SERVER_STATE::SHUTDOWN);
        cout << "error: information of this server received from center server is wrong" << endl;
        
        return;
    }
    
    for(int32 i = 0; i != msg.info_size(); ++i)
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
    
    if(id() == 0)
    {
        state(gabriel::base::SERVER_STATE::SHUTDOWN);
        cout << "error: information of this server received from center server is wrong" << endl;
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
    
}
}

#include "gabriel/main.cpp"
