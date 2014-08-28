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
#include "yaml-cpp/yaml.h"
#include "ace/Dev_Poll_Reactor.h"
#include "gabriel/superrecord/server.hpp"
#include "gabriel/protocol/server/default.pb.h"
#include "gabriel/protocol/server/msg_type.pb.h"

using namespace std;

namespace gabriel {
namespace superrecord {

Server::Server() : m_game_db_pool(this)
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
            set_proc_name_and_log_dir("gabriel_superrecord_server");
            gabriel::base::Server_Connection *tmp = &m_supercenter_connection;
        
            if(m_connector.connect(tmp, ACE_INET_Addr(port, host.c_str())) < 0)
            {
                cout << "error: connect to supercenter server failed" << endl;
            
                return false;
            }
            
            cout << "connect to supercenter server ok" << endl;
        }
        
        {
            YAML::Node superrecord_node = root["superrecord"];
            std::string db = superrecord_node["db"].as<std::string>();
            std::string host = superrecord_node["host"].as<std::string>();
            std::string user = superrecord_node["user"].as<std::string>();
            std::string password = superrecord_node["password"].as<std::string>();
            uint32 game_db_pool_size = superrecord_node["game_db_pool_size"].as<uint32>();

            if(!m_game_db_pool.init(host, db, user, password, game_db_pool_size, std::bind(&Server::handle_db_task, this, _1, _2)))
            {
                cout << "error: game db pool init failed" << endl;
            
                return false;
            }
        }
        
        register_req_to();
    }
    catch(const YAML::Exception &err)
    {
        cout << "error: " << err.what() << endl;

        return false;
    }

    return true;
}

void Server::handle_db_task(gabriel::base::DB_Handler *handler, gabriel::protocol::server::DB_Task *task)
{
    using namespace gabriel::protocol::server;

    if(task->msg_type() == DEFAULT_MSG_TYPE && task->msg_id() == ZONE_INFO_REQ)
    {
        task->set_need_return(true);
        Zone_Info_Rsp rsp;
        mysqlpp::Query query = handler->query("select * from zone_list");
        if(mysqlpp::StoreQueryResult res = query.store())
        {
            for(auto it = res.begin(); it != res.end(); ++it)
            {
                const mysqlpp::Row &row = *it;
                int32 idx = 0;
                Zone_Info *zone_info = rsp.add_zone_info();
                zone_info->set_zone_id(row[idx++]);
                zone_info->mutable_info()->set_server_type(row[idx++]);
                zone_info->mutable_info()->set_server_id(row[idx++]);
                zone_info->mutable_info()->set_inner_addr(row[idx++]);
                zone_info->mutable_info()->set_outer_addr(row[idx++]);
                zone_info->mutable_info()->set_port(row[idx++]);
            }

            rsp.SerializeToString(task->mutable_msg_data());
        }
    }
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
                register_req_to();
                cout << "reconnect to supercenter server ok" << endl;
            }
        }
        
        gabriel::base::sleep_sec(2);
    }
}

void Server::register_req_to()
{
    using namespace gabriel::protocol::server;
    Register_Superrecord msg;
    m_supercenter_connection.send(DEFAULT_MSG_TYPE, REGISTER_SUPERRECORD_SERVER, msg);
}
    
void Server::update_hook()
{
}

void Server::register_msg_handler()
{
    using namespace gabriel::protocol::server;
    using namespace std::placeholders;
    m_server_msg_handler.register_handler(DEFAULT_MSG_TYPE, DB_TASK, std::bind(&Server::handle_db_msg, this, _1, _2, _3));
}

void Server::on_connection_shutdown(gabriel::base::Client_Connection *client_connection)
{
}
    
void Server::fini_hook()
{
    m_game_db_pool.fini();
}

}
}

#include "gabriel/main.cpp"
