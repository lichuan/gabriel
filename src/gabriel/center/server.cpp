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
#include "gabriel/protocol/server/supercenter/msg_type.pb.h"
#include "gabriel/protocol/server/supercenter/default.pb.h"
#include "gabriel/center/server.hpp"

using namespace std;

namespace gabriel {
namespace center {

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
    cout << "error: 与supercenter服务器失去连接" << endl;
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
        if(m_supercenter_connection.lost_connection())
        {
            Server_Connection *tmp = &m_supercenter_connection;

            if(m_connector.connect(tmp, m_supercenter_connection.inet_addr()) < 0)
            {
                cout << "error: 尝试重新连接到supercenter服务器失败" << endl;
            }
            else
            {
                m_supercenter_connection.state(CONNECTION_STATE::CONNECTED);
                register_req();
                cout << "尝试重新连接到supercenter服务器成功" << endl;
            }
        }
        
        gabriel::base::sleep_sec(2);
    }
}
    
void Server::do_decode_server_connection()
{
    m_supercenter_connection.decode();
}

void Server::do_encode_server_connection()
{
    m_supercenter_connection.encode();
}

void Server::do_main_server_connection()
{
    m_supercenter_connection.do_main();
}

void Server::update()
{
    //游戏循环    
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(100, true), true), true);
}
    
void Server::register_req()
{
    using namespace gabriel::protocol::server::supercenter;
    Register msg;
    msg.set_zone_id(zone_id());
    m_supercenter_connection.send(DEFAULT_MSG_TYPE, REGISTER_SERVER, msg);
}
    
int32 Server::init_hook()
{
    zone_id(1); //暂时定为1区, 以后改为配置
    supercenter_inet_addr(20000, "127.0.0.1");    
    gabriel::base::Server_Connection *tmp = &m_supercenter_connection;
    
    if(m_connector.connect(tmp, supercenter_inet_addr()) < 0)
    {
        cout << "error: 连接到supercenter服务器失败" << endl;

        return -1;
    }

    cout << "连接到supercenter服务器成功" << endl;
    register_req();
    
    return 0;
}
    
void Server::register_msg_handler()
{
    using namespace gabriel::protocol::server::supercenter;
    m_supercenter_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_SERVER, this, &Server::register_rsp);
}

void Server::register_rsp(gabriel::base::Server_Connection *server_connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server::supercenter;
    PARSE_MSG(Register_Rsp, msg, data, size);

    for(auto info : m_server_infos)
    {
        delete info;
    }
    
    m_server_infos.clear();
    
    for(uint32 i = 0; i != msg.info_size(); ++i)
    {
        const auto &info = msg.info(i);
        
        if(info.server_type() == gabriel::base::CENTER_SERVER)
        {
            if(id() == 0)
            {
                id(info.server_id());
                
                if(m_acceptor.open(ACE_INET_Addr(info.port(), info.outer_addr().c_str()), ACE_Reactor::instance()) < 0)
                {
                    state(gabriel::base::SERVER_STATE::SHUTDOWN);
                    
                    return;
                }
                
                cout << "启动center服务器成功" << endl;
            }
        }
        else
        {
            auto *new_info = new gabriel::protocol::server::Server_Info;
            new_info->CopyFrom(info);
            m_server_infos.push_back(new_info);
        }
    }    
}
    
void Server::fini_hook()
{
    //停服操作 比如释放资源
    for(auto info : m_server_infos)
    {
        delete info;
    }
}

void Server::handle_connection_msg(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
}

void Server::handle_connection_msg(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    m_supercenter_msg_handler.handle_message(msg_type, msg_id, server_connection, data, size);
}

}
}

int ACE_MAIN (int argc, char *argv[])
{    
    SERVER::instance()->main();

    return 0;
}
