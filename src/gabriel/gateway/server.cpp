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
#include "gabriel/gateway/server.hpp"
#include "gabriel/gateway/message/server_internal.hpp"
#include "gabriel/protocol/server/center/msg_type.pb.h"
#include "gabriel/protocol/server/center/default.pb.h"

using namespace std;

namespace gabriel {
namespace gateway {

Server::Server()
{
}

Server::~Server()
{
}

void Server::on_connection_shutdown(gabriel::base::Client_Connection *client_connection)
{
    //客户端连接掉线
}

void Server::on_connection_shutdown(gabriel::base::Server_Connection *server_connection)
{
    //服务器连接掉线
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
}

void Server::do_decode_server_connection()
{
    m_record_connection.decode();
    m_center_connection.decode();
}

void Server::do_encode_server_connection()
{
    m_record_connection.encode();
    m_center_connection.encode();
}

void Server::do_main_server_connection()
{
    m_record_connection.do_main();
    m_center_connection.do_main();
}

void Server::update()
{
    //游戏循环    
}

int32 Server::init_hook()
{
    ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(5000), true), true);
    m_acceptor.open(ACE_INET_Addr(20200));
    
    return 0;
}

void Server::register_msg_handler()
{
    m_center_connection.register_handler(protocol::server::center::DEFAULT_MSG_TYPE, protocol::server::center::REGISTER_SERVER, &message::register_ret);
}

void Server::fini_hook()
{
    //停服操作 比如释放资源
}

}
}

int ACE_MAIN (int argc, char *argv[])
{    
    SERVER::instance()->main();

    return 0;
}
