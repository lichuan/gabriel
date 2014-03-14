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
#include "ace/Signal.h"
#include "gabriel/base/server.hpp"
#include "gabriel/base/timer.hpp"

namespace gabriel {
namespace base {

Server::Server() : m_acceptor(this), m_connector(this)
{
    m_zone_id = 0;
    m_type = INVALID_SERVER;
}

Server::~Server()
{
}

void Server::main(int argc, char* argv[])
{
    m_proc_name = argv[0];
    
    if(init() < 0)
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
    
int32 Server::init()
{    
    // ACE_Sig_Action no_sigpipe ((ACE_SignalHandler) SIG_IGN);
    // ACE_Sig_Action original_action;
    // no_sigpipe.register_action (SIGPIPE, &original_action);
    
    init_reactor();
    m_connector.open(ACE_Reactor::instance());
    m_thread.add_executor(this, &Server::do_reactor);
    m_thread.add_executor(this, &Server::do_main);
    m_thread.add_executor(this, &Server::do_reconnect);
    register_msg_handler();
    daemon(1, 1);
    m_log_dir = std::string("log") + ACE_DIRECTORY_SEPARATOR_STR;
    
    return init_hook();
}

void Server::handle_connection_msg(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
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

void Server::on_connection_shutdown(Client_Connection *client_connection)
{
    client_connection->state(CONNECTION_STATE::SHUTDOWN);
}
    
void Server::on_connection_shutdown(gabriel::base::Server_Connection *server_connection)
{
}
    
void Server::do_main_server_connection()
{
}

void Server::type(gabriel::base::SERVER_TYPE _type)
{
    m_type = _type;
}

SERVER_TYPE Server::type() const
{
    return m_type;
}
    
void Server::do_main_client_connection()
{
    delete_if([](Client_Connection *client_connection)
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
                  client_connection->close();
              }
        );
}

void Server::do_main()
{
    state(SERVER_STATE::RUNNING);

    while(state() != SERVER_STATE::SHUTDOWN)
    {
        do_main_server_connection();
        do_main_client_connection();
        update();
        sleep_msec(1);
    }
    
    ACE_Reactor::instance()->end_event_loop();
}

void Server::do_reconnect()
{
}

void Server::update()
{
    update_hook();
    TIMER_MGR::instance()->expire();
}
    
void Server::do_reactor()
{
    ACE_Reactor::instance()->run_event_loop();
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
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
