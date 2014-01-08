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
    void main();
    uint32 state() const;
    void state(uint32 _state);
    virtual void dispatch(Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size) = 0;
    virtual void dispatch(Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size) = 0;
    
protected:
    Gabriel_Acceptor<Client_Connection, ACE_SOCK_ACCEPTOR> m_acceptor;
    Gabriel_Connector<Server_Connection, ACE_SOCK_CONNECTOR> m_connector;
    
private:    
    int32 init();
    void fini();
    void run();
    void check_connection();    
    void do_reactor();
    void do_decode();
    void do_encode();
    void do_main();
    virtual void on_client_connection_shutdown(Client_Connection *client_connection);    
    virtual void on_server_connection_shutdown(Server_Connection *server_connection);
    virtual void update();    
    virtual int32 init_hook();
    virtual void fini_hook();
    ID_Allocator<> m_client_connection_id_allocator;
    ID_Allocator<> m_server_connection_id_allocator;
    uint32 m_state;
};

}
}

#endif
