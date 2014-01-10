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

#include "ace/Signal.h"
#include "gabriel/base/server.hpp"

namespace gabriel {
namespace base {

Server::Server() : m_acceptor(this), m_connector(this)
{
}

Server::~Server()
{
}

void Server::main()
{
    if(init() < 0)
    {
        return;
    }

    run();
    fini();
}

uint32 Server::state() const
{
    return m_state;
}

void Server::state(uint32 _state)
{
    m_state = _state;
}
    
int32 Server::init()
{
    ACE_Sig_Action no_sigpipe ((ACE_SignalHandler) SIG_IGN);
    ACE_Sig_Action original_action;
    no_sigpipe.register_action (SIGPIPE, &original_action);
    m_thread.add_executor(this, &Server::do_reactor);
    m_thread.add_executor(this, &Server::do_encode);    
    m_thread.add_executor(this, &Server::do_decode);
    m_thread.add_executor(this, &Server::do_main);
    //daemon(1, 1);
    
    return init_hook();
}

void Server::fini_hook()
{
}

void Server::fini()
{
    m_thread.wait();    
    ACE_Reactor::instance()->close_singleton();
    fini_hook();
}

void Server::run()
{
    m_thread.execute();
}

void Server::on_connection_shutdown(Client_Connection *client_connection)
{
}

void Server::on_connection_shutdown(gabriel::base::Server_Connection *server_connection)
{
}

void Server::do_main_client_connection()
{
    struct CB : Entity_Exec<Client_Connection>
    {
        bool exec(Client_Connection *client_connection)
        {
            client_connection->close();

            return true;
        }
            
        bool can_delete(Client_Connection *client_connection) const
        {
            if(client_connection->state() == CONNECTION_STATE::SHUTDOWN)
            {
                return true;
            }

            client_connection->do_main();
            
            return false;
        }
    };
    
    CB cb;    
    delete_if(cb);
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

void Server::update()
{
}

void Server::do_reactor()
{
    ACE_Reactor::instance()->run_event_loop();
}

void Server::do_encode()
{
    while(state() != SERVER_STATE::SHUTDOWN)
    {
        struct CB : Entity_Exec<Client_Connection>
        {
            bool exec(Client_Connection *client_connection)
            {
                client_connection->encode();
                
                return true;
            }
        };

        CB cb;
        exec_all(cb);
        do_encode_server_connection();
        sleep_msec(1);
    }
}
    
void Server::do_decode()
{
    while(state() != SERVER_STATE::SHUTDOWN)
    {
        struct CB : Entity_Exec<Client_Connection>
        {
            bool exec(Client_Connection *client_connection)
            {
                client_connection->decode();

                return true;
            }
        };

        CB cb;
        exec_all(cb);
        do_decode_server_connection();
        sleep_msec(1);
    }
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
