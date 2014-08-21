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
#include <fstream>
#include <string>
#include <map>
#include "ace/Dev_Poll_Reactor.h"
#include "yaml-cpp/yaml.h"
#include "gabriel/supercenter/server.hpp"
#include "gabriel/protocol/server/msg_type.pb.h"
#include "gabriel/base/db.hpp"
#include "lua.hpp"
#include "lua2cpp.cpp"

using namespace std;

namespace gabriel {
namespace supercenter {

Server::Server()
{
    type(gabriel::base::SUPERCENTER_SERVER);
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
    base::Server::on_connection_shutdown(client_connection);
    
    for(auto iter : m_zone_connections)
    {
        if(iter.second == client_connection)
        {
            m_zone_connections.erase(iter.first);
            cout << "zone: " << iter.first << " center server disconnected from this server" << endl;
            
            break;
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
        YAML::Node root = YAML::LoadFile("lc.yaml");
        cout << YAML::Dump(root);        
        fstream file("lc.yaml", ios::out);
        YAML::Emitter emitter(file);        
        vector<int> ivec = {1,2,3,4,5};
        vector<int> ivec2 = {8,9,10};
        map<int, vector<int>> imap;
        imap[223] = ivec;
        imap[445] = ivec;
        imap[99] = ivec2;        
        emitter << imap;
        emitter << ivec;
    }
    catch(YAML::Exception &err)
    {
        cout << err.what() << endl;

        return false;        
    }
    
    if(m_acceptor.open(ACE_INET_Addr(20000), ACE_Reactor::instance()) < 0)
    {
        cout << "error: start supercenter server failed" << endl;

        return false;
    }
    
    cout << "start supercenter server ok" << endl;
    set_proc_name_and_log_dir("gabriel_supercenter_server");

    /////////////// test script /////////////////////
    lua_State *state = luaL_newstate();
    luaL_openlibs(state);
    register_lua2cpp(state);

    if(luaL_dofile(state, "script/gabriel/script/main.lua") != 0)
    {
        cout << "lua error: " << lua_tostring(state, -1) << endl;
    }    
    /////////////// test script ////////////////////
    
    const uint32 zone_id = 1;    
    {
        gabriel::protocol::server::Server_Info *info = new gabriel::protocol::server::Server_Info;
        info->set_server_id(1);
        info->set_server_type(gabriel::base::CENTER_SERVER);
        info->set_outer_addr("127.0.0.1");
        info->set_inner_addr("127.0.0.1");        
        info->set_port(20002);
        m_server_infos[zone_id].push_back(info);
    }
    {        
        gabriel::protocol::server::Server_Info *info = new gabriel::protocol::server::Server_Info;
        info->set_server_id(2);
        info->set_server_type(gabriel::base::RECORD_SERVER);
        info->set_outer_addr("127.0.0.1");
        info->set_inner_addr("127.0.0.1");
        info->set_port(20003);
        m_server_infos[zone_id].push_back(info);
    }
    {   
        gabriel::protocol::server::Server_Info *info = new gabriel::protocol::server::Server_Info;     
        info->set_server_id(3);
        info->set_server_type(gabriel::base::LOGIN_SERVER);
        info->set_outer_addr("127.0.0.1");
        info->set_inner_addr("127.0.0.1");
        info->set_port(20004);
        m_server_infos[zone_id].push_back(info);
    }
    {        
        gabriel::protocol::server::Server_Info *info = new gabriel::protocol::server::Server_Info;
        info->set_server_id(100);
        info->set_server_type(gabriel::base::GAME_SERVER);
        info->set_outer_addr("127.0.0.1");
        info->set_inner_addr("127.0.0.1");
        info->set_port(20100);
        m_server_infos[zone_id].push_back(info);
    }
    {        
        gabriel::protocol::server::Server_Info *info = new gabriel::protocol::server::Server_Info;
        info->set_server_id(101);
        info->set_server_type(gabriel::base::GAME_SERVER);
        info->set_outer_addr("127.0.0.1");
        info->set_inner_addr("127.0.0.1");
        info->set_port(20101);
        m_server_infos[zone_id].push_back(info);
    }
    {        
        gabriel::protocol::server::Server_Info *info = new gabriel::protocol::server::Server_Info;
        info->set_server_id(200);
        info->set_server_type(gabriel::base::GATEWAY_SERVER);
        info->set_outer_addr("127.0.0.1");
        info->set_inner_addr("127.0.0.1");
        info->set_port(20200);
        m_server_infos[zone_id].push_back(info);
    }
    {        
        gabriel::protocol::server::Server_Info *info = new gabriel::protocol::server::Server_Info;
        info->set_server_id(201);
        info->set_server_type(gabriel::base::GATEWAY_SERVER);
        info->set_outer_addr("127.0.0.1");
        info->set_inner_addr("127.0.0.1");
        info->set_port(20201);
        m_server_infos[zone_id].push_back(info);
    }

    return true;
}

void Server::register_msg_handler()
{
    using namespace gabriel::protocol::server;    
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_CENTER_SERVER, this, &Server::register_req_from);
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, CENTER_ADDR_REQ, this, &Server::center_addr_req_from);
}

void Server::center_addr_req_from(gabriel::base::Client_Connection *client_connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;    
    PARSE_MSG(Center_Addr_Req, msg);
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

    client_connection->send(DEFAULT_MSG_TYPE, CENTER_ADDR_REQ, msg_rsp);
}

void Server::update_hook()
{
}
    
void Server::fini_hook()
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

void Server::register_req_from(gabriel::base::Client_Connection *client_connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;    
    PARSE_MSG(Register_Center, msg);
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

    client_connection->send(DEFAULT_MSG_TYPE, REGISTER_CENTER_SERVER, msg_rsp);
    m_zone_connections[zone_id] = client_connection;
    cout << "zone: " << zone_id << " center server register to this server" << endl;
}
    
void Server::handle_connection_msg(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    m_client_msg_handler.handle_message(msg_type, msg_id, client_connection, data, size);
}

bool Server::handle_connection_msg(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    return true;
}

}
}

#include "gabriel/main.cpp"
