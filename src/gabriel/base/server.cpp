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
    init();
    run();
    fini();
}
    
void Server::init()
{
    ACE_Sig_Action no_sigpipe ((ACE_SignalHandler) SIG_IGN);
    ACE_Sig_Action original_action;
    no_sigpipe.register_action (SIGPIPE, &original_action);
    add_executor(&Server::do_reactor);
    add_executor(&Server::do_encode);    
    add_executor(&Server::do_decode);
    daemon(1, 1);
    init_hook();
}

void Server::init_hook()
{
}

void Server::fini_hook()
{
}

void Server::fini()
{
    ACE_Reactor::instance()->close_singleton();
    wait();
}

void Server::run()
{
    for(;;)
    {
        struct CB : Entity_Exec<Client_Connection>
        {
            bool exec(Client_Connection *client_connection)
            {
                return true;
            }
        };

        CB cb;
        exec_all(cb);
    }    
}

void Server::do_reactor()
{
    ACE_Reactor::instance()->run_event_loop();
}

void Server::do_encode()
{
    for(;;)
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
    }
}

void Server::do_decode()
{
    for(;;)
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
    }
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
}

void Server::add_client_connection(Client_Connection *client_connection)
{
    int unique_id = 0;
    while(get_entity(unique_id = m_client_connection_id_allocator.new_id()) != NULL);
    client_connection->id(unique_id);
    add_entity(client_connection);
}
    
}
}
