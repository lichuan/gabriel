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

#include "ace/Dev_Poll_Reactor.h"
#include "gabriel/game/server.hpp"

namespace gabriel {
namespace game {

Server::Server()
{
}

Server::~Server()
{
}

void Server::dispatch(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
}

void Server::dispatch(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
}

void Server::on_connection_shutdown(gabriel::base::Client_Connection *client_connection)
{
}

void Server::on_connection_shutdown(gabriel::base::Server_Connection *server_connection)
{
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
}

void Server::do_decode_server_connection()
{
}

void Server::do_encode_server_connection()
{
}

void Server::do_main_server_connection()
{
}

void Server::update()
{
}

int32 Server::init_hook()
{
    ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(100), true), true);
    m_acceptor.open(ACE_INET_Addr(22228));
    
    return 0;
}

void Server::fini_hook()
{
}

}
}

int ACE_MAIN (int argc, char *argv[])
{
    SERVER::instance()->main();

    return 0;
}



