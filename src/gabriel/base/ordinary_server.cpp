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
#include "gabriel/protocol/server/msg_type.pb.h"
#include "gabriel/protocol/server/default.pb.h"
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
    
bool Ordinary_Server::on_connection_shutdown(gabriel::base::Server_Connection *server_connection)
{
    if(Super::on_connection_shutdown(server_connection))
    {
        return true;
    }
    
    if(server_connection == &m_center_connection)
    {
        cout << "error: disconnected from center server" << endl;
        
        return true;
    }
    
    return false;    
}
    
void Ordinary_Server::do_reconnect()
{
    if(m_supercenter_connection.lost_connection())
    {
        Server_Connection *tmp = &m_supercenter_connection;

        if(m_connector.connect(tmp, m_supercenter_connection.inet_addr()) < 0)
        {
            cout << "error: reconnect to supercenter server failed" << endl;
        }
        else
        {
            cout << "reconnect to supercenter server ok" << endl;
        }
    }
    
    if(m_center_connection.lost_connection())
    {
        Server_Connection *tmp = &m_center_connection;
            
        if(m_connector.connect(tmp, m_center_connection.inet_addr()) < 0)
        {
            cout << "error: reconnect to center server failed" << endl;
        }
        else
        {
            cout << "reconnect to center server ok" << endl;
            register_req_to();
        }
    }
}
    
void Ordinary_Server::register_req_to()
{
    using namespace gabriel::protocol::server;    
    Register_Ordinary msg;
    msg.set_server_id(id());
    msg.set_server_type(type());
    m_center_connection.send(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, msg);
}

void Ordinary_Server::do_main_on_server_connection()
{
    Super::do_main_on_server_connection();
    m_center_connection.do_main();
}

bool Ordinary_Server::init_hook()
{
    gabriel::base::Server_Connection *tmp = &m_supercenter_connection;
    
    if(m_connector.connect(tmp, m_supercenter_addr) < 0)
    {
        cout << "error: connect to supercenter server failed" << endl;

        return false;        
    }
    
    cout << "connect to supercenter server ok" << endl;
    using namespace gabriel::protocol::server;    
    Center_Addr_Req msg;
    msg.set_zone_id(zone_id());
    m_supercenter_connection.send(DEFAULT_MSG_TYPE, CENTER_ADDR_REQ, msg);

    return true;
}
    
void Ordinary_Server::register_msg_handler()
{
    using namespace gabriel::protocol::server;
    using namespace std::placeholders;    
    m_server_msg_handler.register_handler(DEFAULT_MSG_TYPE, CENTER_ADDR_REQ, std::bind(&Ordinary_Server::center_addr_rsp, this, _1, _2, _3));
}

void Ordinary_Server::center_addr_rsp(gabriel::base::Connection *connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;    
    PARSE_MSG(Center_Addr_Rsp, msg);
    gabriel::base::Server_Connection *tmp = &m_center_connection;
    
    if(m_connector.connect(tmp, ACE_INET_Addr(msg.info().port(), msg.info().inner_addr().c_str())) < 0)
    {
        cout << "error: connect to center server failed" << endl;
        state(gabriel::base::SERVER_STATE::SHUTDOWN);

        return;
    }
    
    cout << "connect to center server ok" << endl;
    register_req_to();
}

}
}
