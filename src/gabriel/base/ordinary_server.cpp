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
 *   @date: 2014-01-24 12:23:21                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include "ace/Dev_Poll_Reactor.h"
#include "gabriel/protocol/server/supercenter/msg_type.pb.h"
#include "gabriel/protocol/server/supercenter/default.pb.h"
#include "gabriel/protocol/server/center/msg_type.pb.h"
#include "gabriel/protocol/server/center/default.pb.h"
#include "gabriel/base/ordinary_server.hpp"

using namespace std;

namespace gabriel {
namespace base {

Ordinary_Server::Ordinary_Server()
{
}

Ordinary_Server::~Ordinary_Server()
{
}
    
void Ordinary_Server::on_connection_shutdown(gabriel::base::Server_Connection *server_connection)
{
    //服务器连接掉线
    if(server_connection == &m_center_connection)
    {
        cout << "error: 与center服务器失去连接" << endl;
    }
    else
    {
        on_connection_shutdown_ordinary(server_connection);
    }
}
    
void Ordinary_Server::on_connection_shutdown_ordinary(gabriel::base::Server_Connection *server_connection)
{
}

void Ordinary_Server::reconnect_ordinary()
{
}

void Ordinary_Server::do_reconnect()
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
                cout << "尝试重新连接到center服务器成功" << endl;
                register_req();
            }
        }
        
        reconnect_ordinary();
        gabriel::base::sleep_sec(2);
    }
}

void Ordinary_Server::register_req()
{
    using namespace gabriel::protocol::server::center;
    Register msg;
    msg.set_server_id(id());
    msg.set_server_type(type());
    m_center_connection.send(DEFAULT_MSG_TYPE, REGISTER_SERVER, msg);
}

void Ordinary_Server::do_main_server_connection()
{
    m_center_connection.do_main();
    m_supercenter_connection.do_main();
    do_main_server_connection_ordinary();
}

void Ordinary_Server::do_main_server_connection_ordinary()
{
}

int32 Ordinary_Server::init_hook()
{
    if(init_hook_ordinary() < 0)
    {
        return -1;
    }
    
    gabriel::base::Server_Connection *tmp = &m_supercenter_connection;
    
    if(m_connector.connect(tmp, m_supercenter_addr) < 0)
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
    
void Ordinary_Server::register_msg_handler()
{
    using namespace gabriel::protocol::server::supercenter;
    m_supercenter_msg_handler.register_handler(DEFAULT_MSG_TYPE, CENTER_ADDR_REQ, this, &Ordinary_Server::center_addr_rsp);
    register_msg_handler_ordinary();
}

void Ordinary_Server::register_msg_handler_ordinary()
{
}
    
void Ordinary_Server::center_addr_rsp(gabriel::base::Server_Connection *server_connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server::supercenter;
    PARSE_MSG(Center_Addr, msg);
    m_supercenter_connection.shutdown();
    gabriel::base::Server_Connection *tmp = &m_center_connection;
    
    if(m_connector.connect(tmp, ACE_INET_Addr(msg.info().port(), msg.info().inner_addr().c_str())) < 0)
    {
        cout << "error: 连接到center服务器失败" << endl;
        state(gabriel::base::SERVER_STATE::SHUTDOWN);

        return;
    }
    
    cout << "连接到center服务器成功" << endl;
    register_req();
}

void Ordinary_Server::handle_connection_msg(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    if(server_connection == &m_supercenter_connection)
    {
        m_supercenter_msg_handler.handle_message(msg_type, msg_id, server_connection, data, size);
    }
    else
    {
        handle_connection_msg_ordinary(server_connection, msg_type, msg_id, data, size);
    }
}

}
}
