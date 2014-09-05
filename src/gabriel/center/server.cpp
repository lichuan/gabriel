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
#include "gabriel/center/server.hpp"
#include "gabriel/protocol/server/msg_type.pb.h"

using namespace std;
using namespace gabriel::protocol::server;

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
    if(client_connection == m_record_client)
    {
        m_record_client = NULL;
        cout << "error: record server disconnected from this server" << endl;
        LOG_ERROR("record server disconnected from this server");
    }
    else if(client_connection->type() == gabriel::base::GAME_CLIENT)
    {
        auto iter = m_allocated_map.find(client_connection->id());
        
        if(iter != m_allocated_map.end())
        {
            m_allocated_ids.erase(iter->second);
            cout << "error: game server (id=" << iter->second << ") disconnected from this server" << endl;
            LOG_ERROR("game server (id=%u) disconnected from this server", iter->second);

            for(auto info : m_server_infos)
            {
                if(info->server_id() == iter->second)
                {
                    info->set_conn_id(0);

                    break;
                }
            }
        }
    }
    else if(client_connection->type() == gabriel::base::GATEWAY_CLIENT)
    {
        auto iter = m_allocated_map.find(client_connection->id());
        
        if(iter != m_allocated_map.end())
        {
            m_allocated_ids.erase(iter->second);
            cout << "error: gateway server (id=" << iter->second << ")disconnected from this server" << endl;
            LOG_ERROR("gateway server (id=%u)disconnected from this server", iter->second);

            for(auto info : m_server_infos)
            {
                if(info->server_id() == iter->second)
                {
                    info->set_conn_id(0);

                    break;
                }
            }
        }
    }
    else if(client_connection->type() == gabriel::base::LOGIN_CLIENT)
    {
        cout << "error: login server disconnected from this server" << endl;
        LOG_ERROR("login server disconnected from this server");
    }
    else
    {
        cout << "error: unknown server disconnected from this server" << endl;
        LOG_ERROR("unknown server disconnected from this server");
    }
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
}

void Server::do_reconnect()
{
    Super::do_reconnect();
    
    if(m_supercenter_connection.lost_connection())
    {
        gabriel::base::Server_Connection *tmp = &m_supercenter_connection;

        if(m_connector.connect(tmp, m_supercenter_connection.inet_addr()) < 0)
        {
            cout << "error: reconnect to supercenter server failed" << endl;
            LOG_ERROR("reconnect to supercenter server failed");
        }
        else
        {
            register_req_to();
            cout << "reconnect to supercenter server ok" << endl;
            LOG_INFO("reconnect to supercenter server ok");
        }
    }
}
    
void Server::update_hook()
{
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(100, true), true), true);
}

void Server::register_req_to()
{
    Register_Center msg;
    msg.set_zone_id(zone_id());
    m_supercenter_connection.send(DEFAULT_MSG_TYPE, REGISTER_CENTER_SERVER, msg);
}
    
bool Server::init_hook()
{
    register_req_to();
    
    return true;    
}
    
void Server::register_msg_handler()
{
    using namespace placeholders;    
    m_server_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_CENTER_SERVER, bind(&Server::register_rsp_from, this, _1, _2, _3));
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, bind(&Server::register_req_from, this, _1, _2, _3));
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, SYNC_ONLINE_NUM, bind(&Server::sync_online_num, this, _1, _2, _3));
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, GET_ONE_GATEWAY, bind(&Server::get_one_gateway, this, _1, _2, _3));
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, SYNC_ACCOUNT, bind(&Server::sync_account_rsp, this, _1, _2, _3));
}

void Server::sync_account_rsp(gabriel::base::Connection *connection, void *data, uint32 size)
{
    PARSE_FROM_ARRAY(Sync_Account_Rsp, msg, data, size);
    base::Connection *login_conn = get_connetion(msg.conn_id_1());
    
    if(login_conn != NULL)
    {
        login_conn->send(DEFAULT_MSG_TYPE, SYNC_ACCOUNT, msg);
    }
}

void Server::get_one_gateway(gabriel::base::Connection *connection, void *data, uint32 size)
{
    PARSE_FROM_ARRAY(Get_One_Gateway, msg, data, size);
    uint32 target_num = 100000000;
    Server_Info *target_info = NULL;
    
    for(auto info : m_server_infos)
    {
        if(info->server_type() == gabriel::base::GATEWAY_SERVER)
        {
            uint32 online_num = info->online_num();

            if(online_num < target_num && info->conn_id() > 0)
            {
                target_num = online_num;
                target_info = info;
            }
        }
    }
    
    if(target_info != NULL)
    {
        string key = base::gen_uuid();
        gabriel::base::Connection *gateway = get_connetion(target_info->conn_id());

        if(gateway != NULL)
        {
            Sync_Account sync_msg;
            sync_msg.set_addr(target_info->outer_addr());
            sync_msg.set_port(target_info->port());
            sync_msg.set_conn_id_1(connection->id());
            sync_msg.set_conn_id_2(msg.conn_id());
            sync_msg.set_account(msg.account());
            sync_msg.set_key(key);
            gateway->send(DEFAULT_MSG_TYPE, SYNC_ACCOUNT, sync_msg);
        }
    }
}
    
void Server::sync_online_num(gabriel::base::Connection *connection, void *data, uint32 size)
{
    PARSE_FROM_ARRAY(Sync_Online_Num, msg, data, size);
    auto iter = m_allocated_map.find(connection->id());
    
    if(iter == m_allocated_map.end())
    {
        return;
    }
    
    for(auto info : m_server_infos)
    {
        if(info->server_id() == iter->second)
        {
            info->set_online_num(msg.num());
        }
    }
}

void Server::register_req_from(gabriel::base::Connection *connection, void *data, uint32 size)
{
    PARSE_FROM_ARRAY(Register_Ordinary, msg, data, size);
    Register_Ordinary_Rsp msg_rsp;
    static_cast<gabriel::base::Client_Connection*>(connection)->type(static_cast<gabriel::base::CLIENT_TYPE>(msg.server_type()));
    uint32 conn_id = connection->id();
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
                m_record_client = static_cast<gabriel::base::Client_Connection*>(connection);
                
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
            default:
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
                    if(info->server_id() == msg.server_id() && m_allocated_ids.find(info->server_id()) == m_allocated_ids.end())
                    {
                        msg_rsp.add_info()->CopyFrom(*info);
                        info->set_conn_id(conn_id);
                        m_allocated_ids.insert(info->server_id());
                        m_allocated_map.insert(make_pair(connection->id(), info->server_id()));
                        ACE_OS::sprintf(server_name, "game server (id=%u)", info->server_id());                        
                        found_one = true;
                    }
                }
                else if((info->inner_addr() == connection->ip_addr()
                         || info->inner_addr() == connection->host_name()
                         || info->outer_addr() == connection->ip_addr()
                         || info->outer_addr() == connection->host_name())
                        && m_allocated_ids.find(info->server_id()) == m_allocated_ids.end())
                {
                    msg_rsp.add_info()->CopyFrom(*info);
                    info->set_conn_id(conn_id);
                    m_allocated_ids.insert(info->server_id());
                    m_allocated_map.insert(make_pair(connection->id(), info->server_id()));
                    ACE_OS::sprintf(server_name, "game server (id=%u)", info->server_id());                    
                    found_one = true;
                }
            }
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
                    if(info->server_id() == msg.server_id() && m_allocated_ids.find(info->server_id()) == m_allocated_ids.end())
                    {
                        msg_rsp.add_info()->CopyFrom(*info);
                        info->set_conn_id(conn_id);
                        m_allocated_ids.insert(info->server_id());
                        m_allocated_map.insert(make_pair(connection->id(), info->server_id()));
                        ACE_OS::sprintf(server_name, "gateway server (id=%u)", info->server_id());
                        found_one = true;
                    }
                }
                else if((info->inner_addr() == connection->ip_addr()
                         || info->inner_addr() == connection->host_name()
                         || info->outer_addr() == connection->ip_addr()
                         || info->outer_addr() == connection->host_name())
                        && m_allocated_ids.find(info->server_id()) == m_allocated_ids.end())
                {
                    msg_rsp.add_info()->CopyFrom(*info);
                    info->set_conn_id(conn_id);
                    m_allocated_ids.insert(info->server_id());
                    m_allocated_map.insert(make_pair(connection->id(), info->server_id()));
                    ACE_OS::sprintf(server_name, "gateway server (id=%u)", info->server_id());                    
                    found_one = true;
                }                
            }
        }
    }
    
    cout << "received register request from " << server_name << " ip:" << connection->ip_addr() << " hostname:" << connection->host_name() << endl;
    LOG_INFO("received register request from %s ip:%s hostname:%s", server_name, connection->ip_addr(), connection->host_name());
    connection->send(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, msg_rsp);
}

void Server::register_rsp_from(gabriel::base::Connection *connection, void *data, uint32 size)
{
    PARSE_FROM_ARRAY(Register_Center_Rsp, msg, data, size);
    clear_server_info();
    
    for(int32 i = 0; i != msg.info_size(); ++i)
    {
        const auto &info = msg.info(i);
        
        if(info.server_type() == gabriel::base::CENTER_SERVER)
        {
            if(id() > 0)
            {
                continue;
            }
            
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

}
}

#include "gabriel/main.cpp"
