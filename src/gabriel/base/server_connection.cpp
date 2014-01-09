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
 *   @date: 2013-12-15 23:22:16                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "ace/SOCK_Connector.h"
#include "gabriel/base/server_connection.hpp"
#include "gabriel/base/connector.hpp"
#include "gabriel/base/server.hpp"

namespace gabriel {
namespace base {

Server_Connection::Server_Connection()
{
}
    
Server_Connection::~Server_Connection()
{
}

void Server_Connection::set_addr(uint16 port)
{
    m_addr.set_port_number(port);    
}

void Server_Connection::set_addr(uint16 port, const char *addr)
{
    m_addr.set(port, addr);
}

uint16 Server_Connection::port() const
{
    return m_addr.get_port_number();
}

const char* Server_Connection::ip_addr() const
{
    return m_addr.get_host_addr();    
}

const char* Server_Connection::host_name() const
{
    return m_addr.get_host_name();
}

const ACE_INET_Addr& Server_Connection::inet_addr() const
{
    return m_addr;
}
    
void Server_Connection::dispatch(uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    m_holder->dispatch(this, msg_type, msg_id, data, size);
}

void Server_Connection::on_shutdown()
{
    m_holder->on_connection_shutdown(this);
}

int Server_Connection::open(void *acceptor_or_connector)
{
    typedef Gabriel_Connector<Server_Connection, ACE_SOCK_CONNECTOR> Connector;
    Connector *connector = static_cast<Connector*>(acceptor_or_connector);
    m_holder = connector->holder();
    
    return Connection::open(acceptor_or_connector);
}

int Server_Connection::close(u_long flags)
{
    shutdown();
}

}
}
