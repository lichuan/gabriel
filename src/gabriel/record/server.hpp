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
 *   @date: 2014-01-09 12:40:31                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__RECORD__SERVER
#define GABRIEL__RECORD__SERVER

#include "gabriel/base/db.hpp"
#include "gabriel/base/ordinary_server.hpp"
#include "gabriel/base/message_handler.hpp"

namespace gabriel {
namespace record {

class Server : public gabriel::base::Ordinary_Server
{
public:
    Server();
    virtual ~Server();
    
private:
    typedef Ordinary_Server Super;    
    virtual void register_msg_handler();
    virtual void on_connection_shutdown(gabriel::base::Client_Connection *client_connection);
    virtual bool verify_connection(gabriel::base::Client_Connection *client_connection);
    virtual void update_hook();
    virtual void fini_hook();
    virtual void init_reactor();
    virtual bool init_hook();
    virtual void do_reconnect();    
    void register_rsp_from(gabriel::base::Connection *connection, void *data, uint32 size);
    void handle_db_msg(gabriel::base::Connection *connection, void *data, uint32 size);
    void handle_db_task(gabriel::base::DB_Handler *handler, gabriel::protocol::server::DB_Task *task);
    gabriel::base::DB_Handler_Pool m_game_db_pool;
    gabriel::base::DB_Handler_Pool m_log_db_pool;    
};
    
}
}

typedef ACE_Singleton<gabriel::record::Server, ACE_Null_Mutex> SERVER;

#endif
