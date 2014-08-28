/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      _____       ___   _____   _____    _   _____   _               *
 *     /  ___|     /   | |  _  \ |  _  \  | | | ____| | |              *
 *     | |        / /| | | |_| | | |_| |  | | | |__   | |              *
 *     | |  _    / / | | |  _  { |  _  /  | | |  __|  | |              *
 *     | |_| |  / /  | | | |_| | | | \ \  | | | |___  | |___           *
 *     \_____/ /_/   |_| |_____/ |_|  \_\ |_| |_____| |_____|          *
 *                                                                     *
 *     gabriel is an angel from the Holy Bible, this engine is         *
 *   named gabriel, standing for bring good news to people. the        *
 *   goal of gabriel engine is to help people to develop server        *
 *   for online games, welcome you to join in.                         *
 *                                                                     *
 *   @author: lichuan                                                  *
 *   @qq: 308831759                                                    *
 *   @email: 308831759@qq.com                                          *
 *   @github: https://github.com/lichuan/gabriel                       *
 *   @date: 2014-06-21 10:22:44                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__SUPERRECORD__SERVER
#define GABRIEL__SUPERRECORD__SERVER

#include "gabriel/base/db.hpp"
#include "gabriel/base/server.hpp"
#include "gabriel/base/message_handler.hpp"

namespace gabriel {
namespace superrecord {

class Server : public gabriel::base::Server
{
public:
    Server();
    virtual ~Server();
    
private:
    virtual void register_msg_handler();
    virtual void on_connection_shutdown(gabriel::base::Client_Connection *client_connection);    
    virtual bool verify_connection(gabriel::base::Client_Connection *client_connection);
    virtual void update_hook();
    virtual void fini_hook();
    virtual void do_reconnect();
    virtual void init_reactor();
    virtual bool init_hook();    
    void handle_db_task(gabriel::base::DB_Handler *handler, gabriel::protocol::server::DB_Task *task);
    void handle_db_msg(gabriel::base::Connection *connection, void *data, uint32 size);
    void register_req_to();
    gabriel::base::DB_Handler_Pool m_game_db_pool;
};
    
}
}

typedef ACE_Singleton<gabriel::superrecord::Server, ACE_Null_Mutex> SERVER;

#endif
