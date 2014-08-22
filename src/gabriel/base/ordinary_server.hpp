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
 *   @date: 2014-01-24 11:53:33                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__ORDINARY_SERVER
#define GABRIEL__BASE__ORDINARY_SERVER

#include "gabriel/base/server.hpp"

namespace gabriel {
namespace base {

class Ordinary_Server : public Server
{
protected:
    Ordinary_Server();
    virtual ~Ordinary_Server();
    virtual bool on_connection_shutdown_extra(gabriel::base::Server_Connection *server_connection);
    virtual void do_main_on_server_connection_extra();
    virtual void do_reconnect();
    virtual bool init_hook();
    virtual void register_msg_handler();
    Server_Connection m_center_connection;
    
private:
    void center_addr_rsp(gabriel::base::Connection *connection, void *data, uint32 size);
    virtual void do_reconnect_extra();
    void register_req_to();
};
    
}
}
        
#endif
