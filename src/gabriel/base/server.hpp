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
 *   @date: 2013-11-29 09:00:34                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__SERVER
#define GABRIEL__BASE__SERVER

#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Connector.h"
#include "gabriel/base/connector.hpp"
#include "gabriel/base/acceptor.hpp"
#include "gabriel/base/acceptor.hpp"
#include "gabriel/base/thread.hpp"
#include "gabriel/base/client_connection.hpp"
#include "gabriel/base/server_connection.hpp"

namespace gabriel {
namespace base {

class Server : public Entity_Manager<Client_Connection, KEY_ID>, public Thread<Server>
{
public:
    Server();
    virtual ~Server();
    void add_client_connection(Client_Connection *client_connection);
    virtual bool verify_connection(Client_Connection *client_connection);
    
protected:
    void init();
    void fini();
    void run();
    Gabriel_Acceptor<Client_Connection, ACE_SOCK_ACCEPTOR> m_acceptor;
    Gabriel_Connector<Server_Connection, ACE_SOCK_CONNECTOR> m_connector;
    
private:
    void reactor_thread();
    void decode_thread();    
    void encode_thread();
    virtual void init_hook() = 0;
    virtual void fini_hook() = 0;
    ID_Allocator<> m_client_connection_id_allocator;
    ID_Allocator<> m_server_connection_id_allocator;    
    //Entity_Manager<Server_Connection, KEY_ID> m_server_connections;    
};

}
}

#endif
