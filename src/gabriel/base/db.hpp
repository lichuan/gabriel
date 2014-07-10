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

#include "mysql++.h"
#include "gabriel/base/thread.hpp"

namespace gabriel {
namespace base {

class DB_Task
{
public:
    DB_Task();
    ~DB_Task();

private:
    uint32 m_seq;    
};

class DB_Handler : public mysqlpp::Connection, public Thread<DB_Handler>
{
public:
    DB_Handler()
    {
    }

    void run()
    {
    }
};
    
class DB_Handler_Pool
{
public:
    DB_Handler_Pool();
    void init(const char *db_host, const char *db_name, const char *db_user, const char *db_password, uint32 num_of_handler);
};
    
}
}
        
#endif
