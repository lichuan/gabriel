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
}

bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
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

int32 Server::init_hook()
{
    // if(m_acceptor.open(ACE_INET_Addr(20001), ACE_Reactor::instance()) < 0)
    // {
    //     return -1;
    // }
    
    m_supercenter_connection.set_addr(20000);
    gabriel::base::Server_Connection *tmp = &m_supercenter_connection;
    
    if(m_connector.connect(tmp, m_supercenter_connection.inet_addr()) < 0)
    {
        cout << "error: 连接到超级中心服务器失败" << endl;

        return -1;
    }

    cout << "连接到超级中心服务器成功" << endl;

    //register self to supercenter
    using namespace gabriel::protocol::server::supercenter;
    Register msg;
    msg.mutable_info()->set_zone_id(122);
    msg.mutable_info()->set_outer_addr("192.168.7.8");
    char buf[gabriel::base::MSG_SERIALIZE_BUF_HWM];
    const uint32 byte_size = msg.ByteSize();
    msg.SerializeToArray(buf, byte_size);
    m_supercenter_connection.send(DEFAULT_MSG_TYPE, REGISTER_SERVER, buf, byte_size);
    
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
    Register msg;
    msg.ParseFromArray(data, size);
    cout << "zone: " << msg.info().zone_id() << endl;
    cout << "addr: " << msg.info().outer_addr() << endl;
}

void Server::fini_hook()
{
    //停服操作 比如释放资源
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
}

}
}

int ACE_MAIN (int argc, char *argv[])
{    
    SERVER::instance()->main();

    return 0;
}
