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
 *   @date: 2013-12-15 23:21:42                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "ace/SOCK_Acceptor.h"
#include "gabriel/base/server.hpp"
#include "gabriel/base/acceptor.hpp"
#include "gabriel/base/client_connection.hpp"

namespace gabriel {
namespace base {

Client_Connection::Client_Connection()
{
    m_type = INVALID_CLIENT;
}

Client_Connection::~Client_Connection()
{
}

void Client_Connection::type(gabriel::base::CLIENT_TYPE _type)
{
    m_type = _type;    
}

CLIENT_TYPE Client_Connection::type() const
{
    return m_type;
}
    
void Client_Connection::on_shutdown()
{
    m_holder->on_connection_shutdown(this);
}

int Client_Connection::open(void *acceptor_or_connector)
{
    typedef Acceptor<Client_Connection, ACE_SOCK_ACCEPTOR> Acceptor;
    Acceptor *acceptor = static_cast<Acceptor*>(acceptor_or_connector);
    m_holder = acceptor->holder();
    int open_ret = Connection::open(acceptor_or_connector);

    if(open_ret < 0)
    {
        return -1;
    }
    
    if(!m_holder->verify_connection(this))
    {
        return -1;
    }

    m_holder->add_connection(this);

    return 0;    
}

void Client_Connection::dispatch(uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    m_holder->handle_connection_msg(this, msg_type, msg_id, data, size);
}

void Client_Connection::destroy_this_ref()
{
    close();
}

}
}
