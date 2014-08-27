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
#include "yaml-cpp/yaml.h"
#include "gabriel/record/server.hpp"
#include "gabriel/protocol/server/msg_type.pb.h"
#include "gabriel/protocol/server/default.pb.h"

using namespace std;

namespace gabriel {
namespace record {

Server::Server() : m_game_db_pool(this), m_log_db_pool(this)
{
    type(gabriel::base::RECORD_SERVER);
}

Server::~Server()
{
}

void Server::on_connection_shutdown(gabriel::base::Client_Connection *client_connection)
{
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
}

void Server::do_reconnect()
{
    while(state() != gabriel::base::SERVER_STATE::SHUTDOWN)
    {
        Super::do_reconnect();
        gabriel::base::sleep_sec(2);
    }
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(100, true), true), true);
}

bool Server::init_hook()
{
    try
    {
        using namespace std::placeholders;
        YAML::Node root = YAML::LoadFile("resource/config.yaml");
        
        {
            YAML::Node supercenter_node = root["supercenter"];
            std::string host = supercenter_node["host"].as<std::string>();
            uint16 port = supercenter_node["port"].as<uint16>();
            zone_id(root["zone_id"].as<uint32>());
            m_supercenter_addr.set(port, host.c_str());
        }
        
        {
            YAML::Node record_node = root["record"];
            std::string db = record_node["db"].as<std::string>();
            std::string host = record_node["host"].as<std::string>();
            std::string user = record_node["user"].as<std::string>();
            std::string password = record_node["password"].as<std::string>();
            uint32 game_db_pool_size = record_node["game_db_pool_size"].as<uint32>();
            uint32 log_db_pool_size = record_node["log_db_pool_size"].as<uint32>();
            
            if(!m_game_db_pool.init(host, db, user, password, game_db_pool_size, std::bind(&Server::handle_db_task, this, _1, _2)))
            {
                cout << "error: game db pool init failed" << endl;
            
                return false;
            }

            if(!m_log_db_pool.init(host, db, user, password, log_db_pool_size, std::bind(&Server::handle_db_task, this, _1, _2)))
            {
                cout << "error: log db pool init failed" << endl;
                
                return false;
            }
        }
    }
    catch(const YAML::Exception &err)
    {
        cout << err.what() << endl;

        return false;        
    }

    return Super::init_hook();
}

void Server::update_hook()
{
}

void Server::register_msg_handler()
{
    using namespace gabriel::protocol::server;
    using namespace std::placeholders;
    Super::register_msg_handler();
    m_server_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, std::bind(&Server::register_rsp_from, this, _1, _2, _3));
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, DB_TASK, std::bind(&Server::handle_db_msg, this, _1, _2, _3));
}

void Server::handle_db_task(gabriel::base::DB_Handler *handler, gabriel::protocol::server::DB_Task *task)
{
    using namespace gabriel::protocol::server;
}

void Server::handle_db_msg(gabriel::base::Connection *connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;
    DB_Task *task = new DB_Task;

    if(!task->ParseFromArray(data, size))
    {
        delete task;

        return;
    }

    task->set_start_tick(gabriel::base::get_usec_tick());
    
    if(task->pool_id() == gabriel::base::DB_Handler_Pool::GAME_POOL)
    {
        m_game_db_pool.add_task(connection, task);
    }
    else if(task->pool_id() == gabriel::base::DB_Handler_Pool::LOG_POOL)
    {
        m_log_db_pool.add_task(connection, task);
    }    
}
    
void Server::register_rsp_from(gabriel::base::Connection *connection, void *data, uint32 size)
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
    
void Server::fini_hook()
{
}

}
}

#include "gabriel/main.cpp"
