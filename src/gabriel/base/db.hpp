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
 *   @date: 2014-03-23 20:10:05                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__DB
#define GABRIEL__BASE__DB

#include <utility>
#include "mysql++.h"
#include "ace/Message_Queue_T.h"
#include "gabriel/base/thread.hpp"
#include "gabriel/base/connection.hpp"
#include "gabriel/protocol/server/msg_type.pb.h"
#include "gabriel/protocol/server/default.pb.h"

namespace gabriel {
namespace base {

class DB_Handler_Pool;
class Server;
    
class DB_Handler : public mysqlpp::Connection, public Thread<DB_Handler>
{
public:
    DB_Handler(DB_Handler_Pool *holder);
    void do_task();    
    void add_task(gabriel::base::Connection *connection, gabriel::protocol::server::DB_Task *task);
    
private:
    ACE_Message_Queue_Ex<std::pair<gabriel::base::Connection*, gabriel::protocol::server::DB_Task*>, ACE_MT_SYNCH> m_task_queue;
    DB_Handler_Pool *m_holder;
    Server *m_server;    
};
    
class DB_Handler_Pool
{
public:
    friend class DB_Handler;
    enum
    {
        LOG_POOL = 1,
        GAME_POOL = 2,
    };
    
    DB_Handler_Pool(Server *holder);
    bool init(std::string host, std::string db, std::string user, std::string password, uint32 num_of_handler, std::function<void(DB_Handler *handler, gabriel::protocol::server::DB_Task*)> func);
    void fini();
    void add_task(gabriel::base::Connection *connection, gabriel::protocol::server::DB_Task *task);
    void wait();
    
private:
    std::vector<DB_Handler*> m_handlers;
    uint32 m_num_of_handler; //cache
    Server *m_holder;    
    std::function<void(DB_Handler *handler, gabriel::protocol::server::DB_Task*)> m_func;
};
    
}
}
        
#endif
