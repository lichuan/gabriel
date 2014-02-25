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

#ifndef GABRIEL__CENTER__SERVER
#define GABRIEL__CENTER__SERVER

#include <vector>
#include <set>
#include <map>
#include "google/protobuf/message.h"
#include "gabriel/base/server.hpp"
#include "gabriel/base/message_handler.hpp"
#include "gabriel/protocol/server/public.pb.h"

namespace gabriel {
namespace center {

class Server : public gabriel::base::Server
{
public:
    Server();
    virtual ~Server();
    
private:
    virtual void on_connection_shutdown(gabriel::base::Client_Connection *client_connection);
    virtual void on_connection_shutdown(gabriel::base::Server_Connection *server_connection);
    virtual bool verify_connection(gabriel::base::Client_Connection *client_connection);
    virtual void do_main_server_connection();
    virtual void do_reconnect();
    virtual void update_hook();
    virtual int32 init_hook();
    virtual void fini_hook();
    virtual void init_reactor();    
    virtual void register_msg_handler();
    virtual void handle_connection_msg(gabriel::base::Client_Connection *client_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size);    
    virtual void handle_connection_msg(gabriel::base::Server_Connection *server_connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size);
    void register_rsp(gabriel::base::Server_Connection *server_connection, void *data, uint32 size);
    void register_req(gabriel::base::Client_Connection *client_connection, void *data, uint32 size);
    void send_to_record(uint32 msg_type, uint32 msg_id, google::protobuf::Message &msg);    
    void register_req();
    void clear_server_info();    
    gabriel::base::Server_Connection m_supercenter_connection;
    gabriel::base::Message_Handler<Server, gabriel::base::Server_Connection> m_supercenter_msg_handler;
    gabriel::base::Message_Handler<Server, gabriel::base::Client_Connection> m_client_msg_handler;
    std::vector<gabriel::protocol::server::Server_Info*> m_server_infos;
    gabriel::base::Client_Connection *m_record_client;
    std::set<uint32> m_allocated_gateway_ids;
    std::set<uint32> m_allocated_game_ids;
    std::map<uint32, uint32> m_allocated_game_map;
    std::map<uint32, uint32> m_allocated_gateway_map;
};
    
}
}

typedef ACE_Singleton<gabriel::center::Server, ACE_Null_Mutex> SERVER;

#endif
