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

#include <set>
#include <map>
#include <vector>
#include "google/protobuf/message.h"
#include "gabriel/base/server.hpp"
#include "gabriel/base/message_handler.hpp"
#include "gabriel/protocol/server/default.pb.h"

namespace gabriel {
namespace center {

class Server : public gabriel::base::Server
{
public:
    Server();
    virtual ~Server();
    
private:
    typedef gabriel::base::Server Super;
    virtual void on_connection_shutdown(gabriel::base::Client_Connection *client_connection);
    virtual bool verify_connection(gabriel::base::Client_Connection *client_connection);
    virtual void do_reconnect();
    virtual void update_hook();
    virtual bool init_hook();
    virtual void fini_hook();
    virtual void init_reactor();    
    virtual void register_msg_handler();
    void register_rsp_from(gabriel::base::Connection *connection, void *data, uint32 size);
    void register_req_from(gabriel::base::Connection *connection, void *data, uint32 size);
    void sync_online_num(gabriel::base::Connection *connection, void *data, uint32 size);
    void get_one_gateway(gabriel::base::Connection *connection, void *data, uint32 size);
    void sync_account_rsp(gabriel::base::Connection *connection, void *data, uint32 size);
    void register_req_to();
    void clear_server_info();
    std::vector<gabriel::protocol::server::Server_Info*> m_server_infos;
    gabriel::base::Client_Connection *m_record_client;
    std::set<uint32> m_allocated_ids;
    std::map<uint32, uint32> m_allocated_map;
};
    
}
}

typedef ACE_Singleton<gabriel::center::Server, ACE_Null_Mutex> SERVER;

#endif
