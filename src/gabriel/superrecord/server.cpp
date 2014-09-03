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
    return true;
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(1000, true), true), true);
}

bool Server::init_hook()
{
    try
    {
        using namespace placeholders;
        YAML::Node root = YAML::LoadFile("resource/config.yml");
        YAML::Node superrecord_node = root["superrecord"];
        string host = superrecord_node["host"].as<string>();
        uint16 port = superrecord_node["port"].as<uint16>();
        set_proc_name_and_log_dir("gabriel_superrecord_server");
        
        if(m_acceptor.open(ACE_INET_Addr(port, host.c_str()), ACE_Reactor::instance()) < 0)
        {
            cout << "error: start superrecord server failed" << endl;

            return false;
        }
        
        cout << "start superrecord server ok" << endl;
        string db = superrecord_node["db"].as<string>();
        string user = superrecord_node["user"].as<string>();
        string password = superrecord_node["password"].as<string>();
        uint32 game_db_pool_size = superrecord_node["game_db_pool_size"].as<uint32>();
        
        if(!m_game_db_pool.init(host, db, user, password, game_db_pool_size, bind(&Server::handle_db_task, this, _1, _2)))
        {
            cout << "error: game db pool init failed" << endl;
            
            return false;
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
    
    switch(task->msg_type())
    {
    case DEFAULT_MSG_TYPE:
        handle_default_msg(handler, task);
        break;
    default:
        break;
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
    using namespace placeholders;
    m_server_msg_handler.register_handler(DEFAULT_MSG_TYPE, DB_TASK, bind(&Server::handle_db_msg, this, _1, _2, _3));
    m_client_msg_handler.register_handler(DEFAULT_MSG_TYPE, DB_TASK, bind(&Server::handle_db_msg, this, _1, _2, _3));
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
