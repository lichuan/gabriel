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

#ifndef GABRIEL__GAME__SERVER
#define GABRIEL__GAME__SERVER

#include "gabriel/base/server.hpp"
#include "gabriel/base/message_handler.hpp"

namespace gabriel {
namespace game {

class Server : public gabriel::base::Server
{
public:
    Server();
    virtual ~Server();
    
private:
    virtual void on_connection_shutdown(gabriel::base::Client_Connection *client_connection);
    virtual void on_connection_shutdown(gabriel::base::Server_Connection *server_connection);
    virtual bool verify_connection(gabriel::base::Client_Connection *client_connection);
    virtual void do_decode_server_connection();
    virtual void do_encode_server_connection();
    virtual void do_main_server_connection();
    virtual void update();
    virtual int32 init_hook();
    virtual void fini_hook();
    virtual void register_msg_handler();
    gabriel::base::Server_Connection m_center_connection;
    gabriel::base::Server_Connection m_record_connection;
};
    
}
}

typedef ACE_Singleton<gabriel::game::Server, ACE_Null_Mutex> SERVER;

#endif
