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
#include "gabriel/record/server.hpp"
#include "gabriel/protocol/server/game/msg_type.pb.h"
#include "gabriel/protocol/server/supercenter/msg_type.pb.h"
#include "gabriel/protocol/server/supercenter/default.pb.h"
#include "gabriel/protocol/server/public.pb.h"

using namespace std;

namespace gabriel {
namespace record {

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
    if(server_connection == &m_center_connection)
    {
        cout << "error: 与center服务器失去连接" << endl;
    }
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
}

void Server::do_reconnect()
{
    using namespace gabriel::base;
    
    while(state() != gabriel::base::SERVER_STATE::SHUTDOWN)
    {
        if(m_center_connection.lost_connection())
        {
            Server_Connection *tmp = &m_center_connection;
            
            if(m_connector.connect(tmp, m_center_connection.inet_addr()) < 0)
            {
                cout << "error: 尝试重新连接到center服务器失败" << endl;
            }
            else
            {
                m_center_connection.state(CONNECTION_STATE::CONNECTED);
                cout << "尝试重新连接到center服务器成功" << endl;
            }
        }
        
        gabriel::base::sleep_sec(2);
    }
}

void Server::do_decode_server_connection()
{
    m_center_connection.decode();
    m_supercenter_connection.decode();
}

void Server::do_encode_server_connection()
{
    m_center_connection.encode();
    m_supercenter_connection.encode();    
}

void Server::do_main_server_connection()
{
    m_center_connection.do_main();
    m_supercenter_connection.do_main();
}

void Server::update()
{
    //游戏循环    
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(100, true), true), true);
    m_center_connection.reactor(ACE_Reactor::instance());
}

int32 Server::init_hook()
{
    zone_id(1);
    supercenter_inet_addr(20000, "127.0.0.1");
    gabriel::base::Server_Connection *tmp = &m_supercenter_connection;

    if(m_connector.connect(tmp, supercenter_inet_addr()) < 0)
    {
        cout << "error: 连接到supercenter服务器失败" << endl;

        return -1;
    }
    
    cout << "连接到supercenter服务器成功" << endl;
    using namespace gabriel::protocol::server::supercenter;
    Center_Addr_Req msg;
    msg.set_zone_id(zone_id());
    m_supercenter_connection.send(DEFAULT_MSG_TYPE, CENTER_ADDR_REQ, msg);
    
    return 0;
}

void Server::register_msg_handler()
{
    using namespace gabriel::protocol::server::supercenter;
    m_supercenter_msg_handler.register_handler(DEFAULT_MSG_TYPE, CENTER_ADDR_REQ, this, &Server::center_addr_rsp);
}

void Server::center_addr_rsp(gabriel::base::Server_Connection *server_connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server::supercenter;
    PARSE_MSG(Center_Addr, msg, data, size);
    cout << "成功获取center服务器的地址" << endl;
    m_supercenter_connection.shutdown();
    gabriel::base::Server_Connection *tmp = &m_center_connection;
    
    if(m_connector.connect(tmp, ACE_INET_Addr(msg.info().port(), msg.info().outer_addr().c_str())) < 0)
    {
        cout << "error: 连接到center服务器失败" << endl;
        state(gabriel::base::SERVER_STATE::SHUTDOWN);

        return;
    }
    
    cout << "连接到center服务器成功" << endl;
}

void Server::handle_connection_msg(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
}

void Server::handle_connection_msg(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    if(server_connection == &m_supercenter_connection)
    {
        m_supercenter_msg_handler.handle_message(msg_type, msg_id, server_connection, data, size);
    }
    else
    {
        m_center_msg_handler.handle_message(msg_type, msg_id, server_connection, data, size);
    }
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
