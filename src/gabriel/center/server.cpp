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
#include "gabriel/protocol/server/msg_type.pb.h"
#include "gabriel/center/server.hpp"

using namespace std;

namespace gabriel {
namespace center {

Server::Server()
{
    m_record_client = NULL;
    type(gabriel::base::CENTER_SERVER);
}

Server::~Server()
{
}

void Server::on_connection_shutdown(gabriel::base::Client_Connection *client_connection)
{
    gabriel::base::Server::on_connection_shutdown(client_connection);
    
    if(client_connection == m_record_client)
    {
        m_record_client = NULL;
        cout << "record server disconnected from this server" << endl;
    }
    else if(client_connection->type() == gabriel::base::GAME_CLIENT)
    {
        auto iter = m_allocated_game_map.find(client_connection->id());
        
        if(iter != m_allocated_game_map.end())
        {
            m_allocated_game_ids.erase(iter->second);
            cout << "game server (id=" << iter->second << ") disconnected from this server" << endl;
        }
    }
    else if(client_connection->type() == gabriel::base::GATEWAY_CLIENT)
    {
        auto iter = m_allocated_gateway_map.find(client_connection->id());

        if(iter != m_allocated_gateway_map.end())
        {
            m_allocated_gateway_ids.erase(iter->second);
            cout << "gateway server (id=" << iter->second << ")disconnected from this server" << endl;
        }
    }
    else
    {
        cout << "login server disconnected from this server" << endl;
    }
}

void Server::on_connection_shutdown(gabriel::base::Server_Connection *server_connection)
{
    cout << "error: disconnected from supercenter server" << endl;
}

void Server::send_to_record(uint32 msg_type, uint32 msg_id, google::protobuf::Message &msg)
{
    if(m_record_client != NULL)
    {
        m_record_client->send(msg_type, msg_id, msg);
    }
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
}

void Server::do_reconnect()
{
    using namespace gabriel::base;
    
    while(state() != gabriel::base::SERVER_STATE::SHUTDOWN)
    {
        if(m_supercenter_connection.lost_connection())
        {
            Server_Connection *tmp = &m_supercenter_connection;

            if(m_connector.connect(tmp, m_supercenter_connection.inet_addr()) < 0)
            {
                cout << "error: reconnect to supercenter server failed" << endl;
            }
            else
            {
                register_req();
                cout << "reconnect to supercenter server ok" << endl;
            }
        }
        
        gabriel::base::sleep_sec(2);
    }
}

void Server::do_main_server_connection()
{
    m_supercenter_connection.do_main();
}

void Server::update_hook()
{
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(100, true), true), true);
}

void Server::register_req()
{
    using namespace gabriel::protocol::server;    
    Register_Center msg;
    msg.set_zone_id(zone_id());
    m_supercenter_connection.send(DEFAULT_MSG_TYPE, REGISTER_CENTER_SERVER, msg);
}
    
int32 Server::init_hook()
{
    zone_id(1);    
    gabriel::base::Server_Connection *tmp = &m_supercenter_connection;
    
    if(m_connector.connect(tmp, ACE_INET_Addr(20001, "106.186.20.182")) < 0)
    {
        cout << "error: connect to supercenter server failed" << endl;

        return -1;
    }

    cout << "connect to supercenter server ok" << endl;
    register_req();
    
    return 0;
}
    
void Server::register_msg_handler()
{
    using namespace gabriel::protocol::server;
    m_supercenter_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_CENTER_SERVER, this, &Server::register_rsp);
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, this, &Server::register_req);
}

void Server::register_req(gabriel::base::Client_Connection *client_connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;    
    PARSE_MSG(Register_Ordinary, msg);
    Register_Ordinary_Rsp msg_rsp;
    client_connection->type(static_cast<gabriel::base::CLIENT_TYPE>(msg.server_type()));
    bool found_one = false;
    char server_name[256] {0};
    
    if(msg.server_type() == gabriel::base::RECORD_SERVER)
    {
        ACE_OS::sprintf(server_name, "record server");
        
        for(auto info : m_server_infos)
        {
            if(info->server_type() == gabriel::base::RECORD_SERVER)
            {
                msg_rsp.add_info()->CopyFrom(*info);
                m_record_client = client_connection;
                
                break;
            }
        }
    }
    else if(msg.server_type() == gabriel::base::LOGIN_SERVER)
    {
        ACE_OS::sprintf(server_name, "login server");        
        
        for(auto info : m_server_infos)
        {
            switch(info->server_type())
            {
            case gabriel::base::RECORD_SERVER:
            case gabriel::base::LOGIN_SERVER:
                msg_rsp.add_info()->CopyFrom(*info);
                break;
            }
        }
    }
    else if(msg.server_type() == gabriel::base::GAME_SERVER)
    {
        for(auto info : m_server_infos)
        {
            if(info->server_type() == gabriel::base::RECORD_SERVER)
            {
                msg_rsp.add_info()->CopyFrom(*info);
            }
            else if(info->server_type() == gabriel::base::GAME_SERVER)
            {
                if(found_one)
                {
                    continue;
                }
                
                if(msg.server_id() > 0)
                {
                    if(info->server_id() == msg.server_id() && m_allocated_game_ids.find(info->server_id()) == m_allocated_game_ids.end())
                    {
                        msg_rsp.add_info()->CopyFrom(*info);
                        m_allocated_game_ids.insert(info->server_id());
                        m_allocated_game_map.insert(std::make_pair(client_connection->id(), info->server_id()));
                        ACE_OS::sprintf(server_name, "game server (id=%u)", info->server_id());                        
                        found_one = true;
                    }
                }
                else if((info->inner_addr() == client_connection->ip_addr()
                         || info->inner_addr() == client_connection->host_name()
                         || info->outer_addr() == client_connection->ip_addr()
                         || info->outer_addr() == client_connection->host_name())
                        && m_allocated_game_ids.find(info->server_id()) == m_allocated_game_ids.end())
                {
                    msg_rsp.add_info()->CopyFrom(*info);
                    m_allocated_game_ids.insert(info->server_id());
                    m_allocated_game_map.insert(std::make_pair(client_connection->id(), info->server_id()));
                    ACE_OS::sprintf(server_name, "game server (id=%u)", info->server_id());                    
                    found_one = true;
                }
            }
        }

        if(!found_one)
        {
            msg_rsp.Clear();
            client_connection->send(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, msg_rsp);
            
            return;
        }
    }
    else if(msg.server_type() == gabriel::base::GATEWAY_SERVER)
    {
        for(auto info : m_server_infos)
        {
            if(info->server_type() == gabriel::base::RECORD_SERVER || info->server_type() == gabriel::base::GAME_SERVER)
            {
                msg_rsp.add_info()->CopyFrom(*info);
            }
            else if(info->server_type() == gabriel::base::GATEWAY_SERVER)
            {
                if(found_one)
                {
                    continue;
                }
                
                if(msg.server_id() > 0)
                {
                    if(info->server_id() == msg.server_id() && m_allocated_gateway_ids.find(info->server_id()) == m_allocated_gateway_ids.end())
                    {
                        msg_rsp.add_info()->CopyFrom(*info);
                        m_allocated_gateway_ids.insert(info->server_id());
                        m_allocated_gateway_map.insert(std::make_pair(client_connection->id(), info->server_id()));
                        ACE_OS::sprintf(server_name, "gateway server (id=%u)", info->server_id());                        
                        found_one = true;
                    }
                }
                else if((info->inner_addr() == client_connection->ip_addr()
                         || info->inner_addr() == client_connection->host_name()
                         || info->outer_addr() == client_connection->ip_addr()
                         || info->outer_addr() == client_connection->host_name())
                        && m_allocated_gateway_ids.find(info->server_id()) == m_allocated_gateway_ids.end())
                {
                    msg_rsp.add_info()->CopyFrom(*info);
                    m_allocated_gateway_ids.insert(info->server_id());
                    m_allocated_gateway_map.insert(std::make_pair(client_connection->id(), info->server_id()));
                    ACE_OS::sprintf(server_name, "gateway server (id=%u)", info->server_id());                    
                    found_one = true;
                }                
            }
        }
        
        if(!found_one)
        {
            msg_rsp.Clear();
            client_connection->send(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, msg_rsp);
            
            return;
        }
    }
    
    cout << "received register request from " << server_name << " ip:" << client_connection->ip_addr() << " hostname:" << client_connection->host_name() << endl;
    client_connection->send(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, msg_rsp);
}

void Server::register_rsp(gabriel::base::Server_Connection *server_connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;    
    PARSE_MSG(Register_Center_Rsp, msg);
    clear_server_info();
    
    for(uint32 i = 0; i != msg.info_size(); ++i)
    {
        const auto &info = msg.info(i);
        
        if(info.server_type() == gabriel::base::CENTER_SERVER)
        {
            if(id() == 0)
            {
                id(info.server_id());
                
                if(m_acceptor.open(ACE_INET_Addr(info.port(), info.inner_addr().c_str()), ACE_Reactor::instance()) < 0)
                {
                    state(gabriel::base::SERVER_STATE::SHUTDOWN);
                    cout << "error: start center server failed" << endl;
                    
                    return;
                }
                
                cout << "start center server ok" << endl;
                set_proc_name_and_log_dir("gabriel_center_server___%u___%u", zone_id(), id());
            }
        }
        else
        {
            auto *new_info = new Server_Info;
            new_info->CopyFrom(info);
            m_server_infos.push_back(new_info);
        }
    }
}

void Server::clear_server_info()
{
    for(auto info : m_server_infos)
    {
        delete info;
    }

    m_server_infos.clear();
}

void Server::fini_hook()
{
    clear_server_info();
}

void Server::handle_connection_msg(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    m_client_msg_handler.handle_message(msg_type, msg_id, client_connection, data, size);    
}

void Server::handle_connection_msg(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    m_supercenter_msg_handler.handle_message(msg_type, msg_id, server_connection, data, size);
}

}
}

#include "gabriel/main.cpp"
