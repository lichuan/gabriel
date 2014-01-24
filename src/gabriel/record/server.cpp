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
#include "gabriel/record/server.hpp"
#include "gabriel/protocol/server/supercenter/msg_type.pb.h"
#include "gabriel/protocol/server/supercenter/default.pb.h"
#include "gabriel/protocol/server/center/msg_type.pb.h"
#include "gabriel/protocol/server/center/default.pb.h"
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

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
}

void Server::update()
{
    //游戏循环    
}

void Server::register_msg_handler_ordinary()
{
    using namespace gabriel::protocol::server::center;
    m_center_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_SERVER, this, &Server::register_rsp);
}

void Server::register_req()
{
    using namespace gabriel::protocol::server::center;
    Register msg;
    msg.set_server_id(id());
    msg.set_server_type(gabriel::base::RECORD_SERVER);
    m_center_connection.send(DEFAULT_MSG_TYPE, REGISTER_SERVER, msg);
}

void Server::handle_connection_msg(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
}

void Server::register_rsp(gabriel::base::Server_Connection *server_connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server::center;
    PARSE_MSG(Register_Rsp, msg);

    if(msg.info_size() <= 0)
    {
        state(gabriel::base::SERVER_STATE::SHUTDOWN);
        cout << "error: 从center服务器接收到的本服务器信息为空" << endl;
        
        return;
    }
    
    const gabriel::protocol::server::Server_Info &info = msg.info(0);
    
    if(id() == 0)
    {
        id(info.server_id());
        
        if(m_acceptor.open(ACE_INET_Addr(info.port(), info.inner_addr().c_str()), ACE_Reactor::instance()) < 0)
        {
            state(gabriel::base::SERVER_STATE::SHUTDOWN);
            cout << "error: 启动record服务器失败" << endl;
            
            return;
        }
    }
    
    cout << "启动record服务器成功" << endl;
}
    
void Server::handle_connection_msg_ordinary(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    if(server_connection == &m_center_connection)
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

#include "gabriel/main.cpp"
