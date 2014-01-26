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

#ifndef GABRIEL__SUPERCENTER__SERVER
#define GABRIEL__SUPERCENTER__SERVER

#include <map>
#include <vector>
#include "gabriel/base/server.hpp"
#include "gabriel/base/message_handler.hpp"
#include "gabriel/protocol/server/public.pb.h"

namespace gabriel {
namespace supercenter {

class Server : public gabriel::base::Server
{
public:
    Server();
    virtual ~Server();
    
private:
    virtual void on_connection_shutdown(gabriel::base::Client_Connection *client_connection);
    virtual int32 init_hook();
    virtual void init_reactor();
    virtual void fini_hook();    
    virtual void register_msg_handler();
    virtual bool verify_connection(gabriel::base::Client_Connection *client_connection);
    virtual void handle_connection_msg(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size);
    void register_req(gabriel::base::Client_Connection *client_connection, void *data, uint32 size);
    void center_addr_req(gabriel::base::Client_Connection *client_connection, void *data, uint32 size);    
    gabriel::base::Message_Handler<Server, gabriel::base::Client_Connection> m_client_msg_handler;
    std::map<uint32, std::vector<gabriel::protocol::server::Server_Info*>> m_server_infos;
    std::map<uint32, gabriel::base::Client_Connection*> m_zone_connections;
};
    
}
}

typedef ACE_Singleton<gabriel::supercenter::Server, ACE_Null_Mutex> SERVER;

#endif
