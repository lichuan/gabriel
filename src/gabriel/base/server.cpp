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
 *   @date: 2013-12-15 23:33:45                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cstdarg>
#include <iostream>
#include "yaml-cpp/yaml.h"
#include "ace/Signal.h"
#include "gabriel/base/timer.hpp"
#include "gabriel/base/server.hpp"

using namespace std;

namespace gabriel {
namespace base {

Server::Server() : m_acceptor(this), m_connector(this)
{
    m_zone_id = 0;
    m_state = SERVER_STATE::INVALID;
    m_main_lua_state = NULL;
}

Server::~Server()
{
}

void Server::main(int argc, char* argv[])
{
    m_proc_name = argv[0];
    
    if(!init())
    {
        return;
    }
    
    run();
    fini();
}

void Server::set_proc_name_and_log_dir(const char *format, ...)
{
    char proc_name[128];
    va_list args_list;
    va_start(args_list, format);
    ACE_OS::vsprintf(proc_name, format, args_list);
    strncpy(m_proc_name, proc_name, 128);
    LOG_MSG::instance()->init(m_log_dir + "log_" + proc_name + ACE_DIRECTORY_SEPARATOR_STR);
}

void Server::init_log()
{
    LOG_MSG::instance()->init();
}
    
uint32 Server::state() const
{
    return m_state;
}

uint32 Server::zone_id() const
{
    return m_zone_id;
}
    
void Server::zone_id(uint32 id)
{
    m_zone_id = id;
}

void Server::state(uint32 _state)
{
    m_state = _state;
}
    
bool Server::init()
{    
    // ACE_Sig_Action no_sigpipe ((ACE_SignalHandler) SIG_IGN);
    // ACE_Sig_Action original_action;
    // no_sigpipe.register_action (SIGPIPE, &original_action);
    init_reactor();
    m_connector.open(ACE_Reactor::instance());
    m_thread.add_executor(std::bind(&Server::do_reactor_thread,this));
    m_thread.add_executor(std::bind(&Server::do_main_thread, this));
    m_thread.add_executor(std::bind(&Server::do_reconnect_thread, this));
    register_msg_handler();
    //daemon(1, 1);
    m_log_dir = std::string("log") + ACE_DIRECTORY_SEPARATOR_STR;
    
    try
    {
        if(m_type != SUPERCENTER_SERVER)
        {
            YAML::Node root = YAML::LoadFile("resource/config.yaml");
            YAML::Node supercenter_node = root["supercenter"];
            std::string host = supercenter_node["host"].as<std::string>();
            uint16 port = supercenter_node["port"].as<uint16>();
            gabriel::base::Server_Connection *tmp = &m_supercenter_connection;
            
            if(m_connector.connect(tmp, ACE_INET_Addr(port, host.c_str())) < 0)
            {
                cout << "error: connect to supercenter server failed" << endl;

                return false;
            }
            
            cout << "connect to supercenter server ok" << endl;
            
            if(m_type != SUPERRECORD_SERVER)
            {
                YAML::Node root = YAML::LoadFile("resource/config.yaml");
                YAML::Node superrecord_node = root["superrecord"];
                std::string host = superrecord_node["host"].as<std::string>();
                uint16 port = superrecord_node["port"].as<uint16>();
                gabriel::base::Server_Connection *tmp = &m_superrecord_connection;
                zone_id(root["zone_id"].as<uint32>());
                
                if(m_connector.connect(tmp, ACE_INET_Addr(port, host.c_str())) < 0)
                {
                    cout << "error: connect to superrecord server failed" << endl;

                    return false;
                }
                
                cout << "connect to superrecord server ok" << endl;
            }
        }
    }
    catch(const YAML::Exception &err)
    {
        cout << err.what() << endl;

        return false;        
    }

    return init_hook();
}
    
void Server::fini()
{
    m_thread.wait();
    fini_hook();
}
    
void Server::run()
{
    m_thread.execute();
}

void Server::handle_connection_msg(Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    m_server_msg_handler.handle_message(msg_type, msg_id, server_connection, data, size);
}

void Server::handle_connection_msg(Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    m_client_msg_handler.handle_message(msg_type, msg_id, client_connection, data, size);
}
    
void Server::do_main_on_server_connection()
{
    m_supercenter_connection.do_main();
    m_superrecord_connection.do_main();    
}

bool Server::on_connection_shutdown(gabriel::base::Server_Connection *server_connection)
{
    if(server_connection == &m_supercenter_connection)
    {
        cout << "error: disconnected from supercenter server" << endl;
        LOG_ERROR("disconnected from supercenter server");
        
        return true;        
    }

    if(server_connection == &m_superrecord_connection)
    {
        cout << "error: disconnected from superrecord server" << endl;
        LOG_ERROR("disconnected from superrecord server");
        
        return true;
    }
    
    return false;
}
    
void Server::type(gabriel::base::SERVER_TYPE _type)
{
    m_type = _type;
}

SERVER_TYPE Server::type() const
{
    return m_type;
}
    
void Server::do_main_on_client_connection()
{
    delete_if([](Client_Connection *client_connection)->bool
              {
                  if(client_connection->state() == CONNECTION_STATE::SHUTDOWN)
                  {
                      return true;
                  }

                  client_connection->do_main();

                  return false;
              },
              [](Client_Connection *client_connection)
              {
                  client_connection->release();
              }
        );
}

void Server::do_main_thread()
{
    state(SERVER_STATE::RUNNING);
    init_log();
    
    while(state() != SERVER_STATE::SHUTDOWN)
    {
        do_main_on_server_connection();
        do_main_on_client_connection();
        update();
        sleep_msec(1);
    }
    
    ACE_Reactor::instance()->end_event_loop();
}

void Server::do_reconnect_thread()
{
    init_log();
    
    while(state() != gabriel::base::SERVER_STATE::SHUTDOWN)
    {
        do_reconnect();
        sleep_sec(2);
    }
}

void Server::do_reconnect()
{
    if(m_superrecord_connection.lost_connection())
    {
        Server_Connection *tmp = &m_superrecord_connection;

        if(m_connector.connect(tmp, m_superrecord_connection.inet_addr()) < 0)
        {
            cout << "error: reconnect to superrecord server failed" << endl;
            LOG_ERROR("reconnect to superrecord server failed");
        }
        else
        {
            cout << "reconnect to superrecord server ok" << endl;
            LOG_INFO("reconnect to superrecord server ok");
        }
    }
}

void Server::update()
{
    update_hook();
    TIMER_MGR::instance()->expire();
}
    
void Server::do_reactor_thread()
{
    ACE_Reactor::instance()->run_event_loop();
}
    
void Server::add_connection(Client_Connection *client_connection)
{
    int unique_id = 0;
    while(get_entity(unique_id = m_connection_id_allocator.new_id()) != NULL);
    client_connection->id(unique_id);
    add_entity(client_connection);
}

}
}
