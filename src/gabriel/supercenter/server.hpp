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
#include "gabriel/protocol/server/default.pb.h"

namespace gabriel {
namespace supercenter {

class Server : public gabriel::base::Server
{
public:
    Server();
    virtual ~Server();
    
private:
    virtual void on_connection_shutdown(gabriel::base::Client_Connection *client_connection);
    virtual bool init_hook();
    virtual void init_reactor();
    virtual void update_hook();
    virtual void fini_hook();
    virtual void do_reconnect();
    virtual void register_msg_handler();
    virtual bool verify_connection(gabriel::base::Client_Connection *client_connection);
    void clear_server_info();    
    void register_req_from_center(gabriel::base::Connection *connection, void *data, uint32 size);
    void register_req_from_superrecord(gabriel::base::Connection *connection, void *data, uint32 size);
    void handle_db_msg(gabriel::base::Connection *connection, void *data, uint32 size);
    void center_addr_req_from(gabriel::base::Connection *connection, void *data, uint32 size);
    void forward_to_superrecord(gabriel::base::Connection *connection, void *data, uint32 size);
    void send_to_superrecord(uint32 msg_type, uint32 msg_id, google::protobuf::Message &msg);    
    gabriel::base::Client_Connection *m_superrecord_client;
    std::map<uint32, std::vector<gabriel::protocol::server::Server_Info*>> m_server_infos;
    std::map<uint32, gabriel::base::Connection*> m_zone_connections;
};
    
}
}

typedef ACE_Singleton<gabriel::supercenter::Server, ACE_Null_Mutex> SERVER;

#endif
