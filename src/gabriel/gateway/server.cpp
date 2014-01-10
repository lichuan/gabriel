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
 *   @date: 2014-01-09 14:41:41                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include "ace/Dev_Poll_Reactor.h"
#include "gabriel/gateway/server.hpp"

using namespace std;

namespace gabriel {
namespace gateway {

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
    return true;    
}

void Server::do_decode_server_connection()
{
    m_record_connection.decode();
    m_center_connection.decode();
    m_game_connection.decode();
}

void Server::do_encode_server_connection()
{
    m_record_connection.encode();
    m_center_connection.encode();
    m_game_connection.encode();
}

void Server::do_main_server_connection()
{
    m_record_connection.do_main();
    m_center_connection.do_main();
    m_game_connection.do_main();
}

void Server::update()
{
    //游戏循环
    gabriel::base::sleep_sec(3);
    cout << "update.............." << endl;    
}

int32 Server::init_hook()
{
    ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(10000), true), true);
    m_game_connection.set_addr(22228);
    gabriel::base::Server_Connection *game_connection = &m_game_connection;
    
    if(m_connector.connect(game_connection, m_game_connection.inet_addr()) < 0)
    {
        cout << "连接到game服务器失败" << endl;

        return -1;
    }

    cout << "连接到game服务器成功" << endl;
    
    return 0;
}

void Server::fini_hook()
{
    //游戏停服
}

}
}

int ACE_MAIN (int argc, char *argv[])
{
    SERVER::instance()->main();
    
    return 0;
}
