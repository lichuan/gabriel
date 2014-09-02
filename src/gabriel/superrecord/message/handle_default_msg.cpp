/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      _____       ___   _____   _____    _   _____   _               *
 *     /  ___|     /   | |  _  \ |  _  \  | | | ____| | |              *
 *     | |        / /| | | |_| | | |_| |  | | | |__   | |              *
 *     | |  _    / / | | |  _  { |  _  /  | | |  __|  | |              *
 *     | |_| |  / /  | | | |_| | | | \ \  | | | |___  | |___           *
 *     \_____/ /_/   |_| |_____/ |_|  \_\ |_| |_____| |_____|          *
 *                                                                     *
 *     gabriel is an angel from the Holy Bible, this engine is         *
 *   named gabriel, standing for bring good news to people. the        *
 *   goal of gabriel engine is to help people to develop server        *
 *   for online games, welcome you to join in.                         *
 *                                                                     *
 *   @author: lichuan                                                  *
 *   @qq: 308831759                                                    *
 *   @email: 308831759@qq.com                                          *
 *   @github: https://github.com/lichuan/gabriel                       *
 *   @date: 2014-08-31 18:40:27                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "gabriel/superrecord/server.hpp"
#include "gabriel/protocol/error.pb.h"
#include "gabriel/protocol/client/default.pb.h"
#include "gabriel/protocol/client/msg_type.pb.h"

using namespace gabriel::protocol;
using namespace gabriel::protocol::server;

namespace gabriel {
namespace superrecord {
namespace message {
namespace {

void handle_zone_info_req(gabriel::base::DB_Handler *handler, DB_Task *task)
{
    Zone_Info_Rsp rsp;
    mysqlpp::Query query = handler->query("select * from zone_list");
    mysqlpp::StoreQueryResult res = query.store();

    if(!res)
    {
        return;
    }
    
    for(auto it = res.begin(); it != res.end(); ++it)
    {
        const mysqlpp::Row &row = *it;
        int32 idx = 0;
        Zone_Info *zone_info = rsp.add_zone_info();
        zone_info->set_zone_id(row[idx++]);
        zone_info->mutable_info()->set_server_type(row[idx++]);
        zone_info->mutable_info()->set_server_id(row[idx++]);
        zone_info->mutable_info()->set_inner_addr(row[idx++]);
        zone_info->mutable_info()->set_outer_addr(row[idx++]);
        zone_info->mutable_info()->set_port(row[idx++]);
    }
    
    rsp.SerializeToString(task->mutable_msg_data());
    task->set_need_return(true);
}
    
void handle_forward_user_default_msg(uint32 msg_id, gabriel::base::DB_Handler *handler, gabriel::protocol::server::Forward_User_Msg &forward_msg, bool &need_return)
{
    if(msg_id == client::REGISTER_ACCOUNT)
    {
        PARSE_FROM_STRING(client::Register_Account, msg, forward_msg.msg_data());
        mysqlpp::Query query = handler->query("select * from account where account = %0q");
        query.parse();
        mysqlpp::StoreQueryResult res = query.store(msg.account());
        
        if(!res)
        {
            return;
        }

        client::Register_Account_Rsp rsp;

        if(res.num_rows() > 0)
        {
            rsp.set_result(ERR_ACCOUNT_EXIST);
        }
        else
        {
            query.reset();
            query << "insert into account values(account, password, email) values(%0q, %1q, %2q)";
            query.parse();
            query.execute(msg.account(), msg.password(), msg.email());
            rsp.set_result(ERR_OK);
        }
        
        rsp.SerializeToString(forward_msg.mutable_msg_data());
        need_return = true;
    }
}
    
void handle_forward_user_msg(gabriel::base::DB_Handler *handler, DB_Task *task)
{
    PARSE_FROM_STRING(Forward_User_Msg, msg, task->msg_data());
    bool need_return = false;
    
    if(msg.msg_type() == client::DEFAULT_MSG_TYPE)
    {
        handle_forward_user_default_msg(msg.msg_id(), handler, msg, need_return);
    }
    
    if(need_return)
    {
        msg.SerializeToString(task->mutable_msg_data());
        task->set_need_return(true);
    }
}

std::map<uint32, void(*)(gabriel::base::DB_Handler*, DB_Task*)> msg_handlers = {
    {ZONE_INFO_REQ, handle_zone_info_req},
    {FORWARD_USER_MSG, handle_forward_user_msg},
};

}
}

void Server::handle_default_msg(gabriel::base::DB_Handler *handler, gabriel::protocol::server::DB_Task *task)
{
    using namespace gabriel::superrecord::message;
    auto iter = msg_handlers.find(task->msg_id());
    
    if(iter == msg_handlers.end())
    {
        return;
    }

    iter->second(handler, task);
}
    
}
}
