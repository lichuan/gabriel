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
#include "gabriel/base/thread.hpp"
#include "gabriel/base/timer.hpp"
#include "gabriel/base/client_connection.hpp"
#include "gabriel/base/server_connection.hpp"
#include "gabriel/base/log.hpp"

namespace gabriel {
namespace base {
    
class Server : public Locked_Entity_Manager<Client_Connection, KEY_ID>, public Entity<>, public Timer_Capability
{
public:
    Server();
    virtual ~Server();
    void add_connection(Client_Connection *client_connection);
    virtual bool verify_connection(Client_Connection *client_connection);
    void main(int argc, char* argv[]);
    uint32 state() const;
    void state(uint32 _state);
    void type(SERVER_TYPE _type);
    SERVER_TYPE type() const;    
    virtual void on_connection_shutdown(Server_Connection *server_connection);
    virtual void on_connection_shutdown(Client_Connection *client_connection);
    virtual void handle_connection_msg(Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size) = 0;
    virtual void handle_connection_msg(Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size) = 0;
    virtual void register_msg_handler() = 0;
    uint32 zone_id() const;
    void zone_id(uint32 id);
    
protected:
    Acceptor<Client_Connection, ACE_SOCK_ACCEPTOR> m_acceptor;
    Connector<Server_Connection, ACE_SOCK_CONNECTOR> m_connector;
    void set_proc_name_and_log_dir(const char *format, ...);    
    std::string m_log_dir;
    
private:    
    int32 init();
    void fini();
    void run();
    void do_reactor();
    void do_main();
    void do_main_client_connection();
    virtual void do_reconnect();
    virtual void do_main_server_connection();
    virtual int32 init_hook() = 0;
    virtual void fini_hook() = 0;
    virtual void init_reactor() = 0;    
    virtual void update_hook() = 0;
    void update();    
    ID_Allocator<> m_connection_id_allocator;
    uint32 m_state;
    SERVER_TYPE m_type;    
    uint32 m_zone_id;
    Thread<Server> m_thread;
    char *m_proc_name;
};

}
}

#endif
