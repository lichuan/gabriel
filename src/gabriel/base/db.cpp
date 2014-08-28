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
 *   @date: 2014-03-23 20:11:36                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include "gabriel/base/db.hpp"
#include "gabriel/base/server.hpp"

using namespace std;

namespace gabriel {
namespace base {
    
DB_Handler::DB_Handler(DB_Handler_Pool *holder)
{
    m_holder = holder;
    m_server = holder->m_holder;    
    m_task_queue.high_water_mark(MSG_QUEUE_HWM);
    m_task_queue.low_water_mark(MSG_QUEUE_LWM);
}

void DB_Handler::add_task(gabriel::base::Connection *connection, gabriel::protocol::server::DB_Task *task)
{
    connection->retain();
    m_task_queue.enqueue_tail(new std::pair<gabriel::base::Connection*, gabriel::protocol::server::DB_Task*>(connection, task));
}

void DB_Handler::do_task()
{
    while(true)
    {
        if(m_task_queue.is_empty())
        {
            if(m_server->state() == SERVER_STATE::SHUTDOWN)
            {
                break;
            }

            sleep_msec(1);
        }
        else
        {
            std::pair<gabriel::base::Connection*, gabriel::protocol::server::DB_Task*> *task_pair;
            m_task_queue.dequeue(task_pair);
            gabriel::base::Connection *connection = task_pair->first;
            gabriel::protocol::server::DB_Task *task = task_pair->second;
            
            try
            {
                m_holder->m_func(this, task);
                task->set_end_tick(get_usec_tick());
                
                if(task->need_return())
                {
                    connection->send(gabriel::protocol::server::DEFAULT_MSG_TYPE, gabriel::protocol::server::DB_TASK, *task);
                }
            }
            catch(const mysqlpp::Exception &err)
            {
                cout << "error: " << err.what() << endl;
                LOG_ERROR("error: %s", err.what());
            }

            connection->release();
            delete task;
            delete task_pair;
        }
    }    
}

DB_Handler_Pool::DB_Handler_Pool(Server *holder)
{
    m_holder = holder;
    m_num_of_handler = 0;    
}

void DB_Handler_Pool::wait()
{
    for(auto iter : m_handlers)
    {
        iter->wait();
    }
}

bool DB_Handler_Pool::init(std::string host, std::string db, std::string user, std::string password, uint32 num_of_handler, std::function<void(DB_Handler *handler, gabriel::protocol::server::DB_Task*)> func)
{
    m_num_of_handler = num_of_handler;
    m_func = func;
    
    try
    {        
        for(uint32 i = 0; i != num_of_handler; ++i)
        {
            DB_Handler *handler = new DB_Handler(this);

            if(!handler->set_option(new mysqlpp::ReconnectOption(true)))
            {
                return false;
            }

            if(!handler->connect(db.c_str(), host.c_str(), user.c_str(), password.c_str()))
            {
                return false;
            }

            handler->start_executor_instantly(std::bind(&DB_Handler::do_task, handler));
            m_handlers.push_back(handler);
        }
    }
    catch(const mysqlpp::Exception &err)
    {
        cout << "error: " << err.what() << endl;

        return false;
    }
    
    return true;
}
    
void DB_Handler_Pool::fini()
{
    wait();

    for(auto iter : m_handlers)
    {
        delete iter;
    }
}

void DB_Handler_Pool::add_task(gabriel::base::Connection *connection, gabriel::protocol::server::DB_Task *task)
{
    uint32 seq = task->seq();
    uint32 idx = 0;
    
    if(seq == 0)
    {
        //this task can be done by any thread in the pool.
        idx = random_between(0, m_num_of_handler - 1);
    }
    else
    {
        //ensure that the same seq task must be done by order.
        idx = (task->seq() - 1) % m_num_of_handler;
    }

    m_handlers[idx]->add_task(connection, task);
}
    
}
}
