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

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include "ace/Dev_Poll_Reactor.h"
#include "gabriel/base/db.hpp"
#include "gabriel/supercenter/server.hpp"
#include "gabriel/protocol/server/msg_type.pb.h"

using namespace std;

namespace gabriel {
namespace supercenter {

Server::Server()
{
    type(gabriel::base::SUPERCENTER_SERVER);
    m_superrecord_client = NULL;
}

Server::~Server()
{
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
}

void Server::on_connection_shutdown(gabriel::base::Client_Connection *client_connection)
{
    if(m_superrecord_client == client_connection)
    {
        m_superrecord_client = NULL;
        cout << "error: superrecord server disconnected from this server" << endl;
        LOG_ERROR("superrecord server disconnected from this server");
    }
    else
    {
        for(auto iter : m_zone_connections)
        {
            if(iter.second == client_connection)
            {
                m_zone_connections.erase(iter.first);
                cout << "error: zone-" << iter.first << " center server disconnected from this server" << endl;
                LOG_ERROR("zone-%u center server disconnected from this server", iter.first);
                
                break;
            }
        }
    }
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(1000, true), true), true);
}
    
bool Server::init_hook()
{
    try
    {
        YAML::Node root = YAML::LoadFile("resource/config.yaml");
        YAML::Node supercenter_node = root["supercenter"];
        std::string host = supercenter_node["host"].as<std::string>();
        uint16 port = supercenter_node["port"].as<uint16>();
        set_proc_name_and_log_dir("gabriel_supercenter_server");
        
        if(m_acceptor.open(ACE_INET_Addr(port, host.c_str()), ACE_Reactor::instance()) < 0)
        {
            cout << "error: start supercenter server failed" << endl;

            return false;
        }
    
        cout << "start supercenter server ok" << endl;
    }
    catch(const YAML::Exception &err)
    {
        cout << err.what() << endl;

        return false;
    }
    
    return true;
}

void Server::do_reconnect()
{
}

void Server::register_msg_handler()
{
    using namespace gabriel::protocol::server;
    using namespace std::placeholders;
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_CENTER_SERVER, std::bind(&Server::register_req_from_center, this, _1, _2, _3));
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, CENTER_ADDR_REQ, std::bind(&Server::center_addr_req_from, this, _1, _2, _3));
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, DB_TASK, std::bind(&Server::handle_db_msg, this, _1, _2, _3));
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, FORWARD_TO_SUPERRECORD, std::bind(&Server::forward_to_superrecord, this, _1, _2, _3));
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_SUPERRECORD_SERVER, std::bind(&Server::register_req_from_superrecord, this, _1, _2, _3));
}
    
void Server::forward_to_superrecord(gabriel::base::Connection *connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;
    PARSE_FROM_ARRAY(DB_Task, msg, data, size);
    msg.set_conn_id(connection->id());
    send_to_superrecord(DEFAULT_MSG_TYPE, DB_TASK, msg);
}

void Server::center_addr_req_from(gabriel::base::Connection *connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;    
    PARSE_FROM_ARRAY(Center_Addr_Req, msg, data, size);
    const uint32 zone_id = msg.zone_id();
    auto iter = m_server_infos.find(zone_id);

    if(iter == m_server_infos.end())
    {
        return;
    }
    
    auto &infos = iter->second;
    Center_Addr_Rsp msg_rsp;
    
    for(auto info : infos)
    {
        if(info->server_type() == gabriel::base::CENTER_SERVER)
        {
            msg_rsp.mutable_info()->CopyFrom(*info);

            break;            
        }
    }

    connection->send(DEFAULT_MSG_TYPE, CENTER_ADDR_REQ, msg_rsp);
}

void Server::update_hook()
{
}

void Server::clear_server_info()
{
    for(auto iter : m_server_infos)
    {
        for(auto &info : iter.second)
        {
            delete info;
        }
    }

    m_server_infos.clear();
}
    
void Server::fini_hook()
{
    clear_server_info();
}

void Server::register_req_from_superrecord(gabriel::base::Connection *connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;
    m_superrecord_client = static_cast<gabriel::base::Client_Connection*>(connection);
    DB_Task task;
    task.set_seq(1);
    task.set_pool_id(gabriel::base::DB_Handler_Pool::GAME_POOL);
    task.set_msg_type(DEFAULT_MSG_TYPE);
    task.set_msg_id(ZONE_INFO_REQ);
    Zone_Info_Req req;
    req.SerializeToString(task.mutable_msg_data());
    m_superrecord_client->send(DEFAULT_MSG_TYPE, DB_TASK, task);
}

void Server::send_to_superrecord(uint32 msg_type, uint32 msg_id, google::protobuf::Message &msg)
{
    if(m_superrecord_client != NULL)
    {
        m_superrecord_client->send(msg_type, msg_id, msg);
    }
}

void Server::handle_db_msg(gabriel::base::Connection *connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;
    PARSE_FROM_ARRAY(DB_Task, msg, data, size);
    uint32 msg_type = msg.msg_type();
    uint32 msg_id = msg.msg_id();
    clear_server_info();
    
    if(msg_type == DEFAULT_MSG_TYPE && msg_id == ZONE_INFO_REQ)
    {
        PARSE_FROM_STRING(Zone_Info_Rsp, rsp, msg.msg_data());
        
        for(int32 i = 0; i != rsp.zone_info_size(); ++i)
        {
            const Zone_Info &zone_info = rsp.zone_info(i);
            Server_Info *server_info = new Server_Info;
            server_info->CopyFrom(zone_info.info());
            m_server_infos[zone_info.zone_id()].push_back(server_info);
        }
        
        cout << "loading zone info from superrecord is done" << endl;
    }
}
    
void Server::register_req_from_center(gabriel::base::Connection *connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;    
    PARSE_FROM_ARRAY(Register_Center, msg, data, size);
    const uint32 zone_id = msg.zone_id();
    auto iter = m_server_infos.find(zone_id);

    if(iter == m_server_infos.end())
    {
        return;
    }
    
    auto &infos = iter->second;
    Register_Center_Rsp msg_rsp;
    
    for(auto info : infos)
    {
        msg_rsp.add_info()->CopyFrom(*info);
    }

    connection->send(DEFAULT_MSG_TYPE, REGISTER_CENTER_SERVER, msg_rsp);
    m_zone_connections[zone_id] = connection;
    cout << "zone-" << zone_id << " center server register to this server" << endl;
    LOG_INFO("zone-%u center server register to this server", zone_id);
}
    
}
}

#include "gabriel/main.cpp"
