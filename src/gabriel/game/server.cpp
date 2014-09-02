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
 *   @date: 2014-01-09 12:48:05                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include "yaml-cpp/yaml.h"
#include "ace/Dev_Poll_Reactor.h"
#include "gabriel/game/server.hpp"
#include "gabriel/protocol/server/default.pb.h"
#include "gabriel/protocol/server/msg_type.pb.h"
#include "lua2cpp.cpp"

using namespace std;

namespace gabriel {
namespace game {

Server::Server()
{
    type(gabriel::base::GAME_SERVER);    
}

Server::~Server()
{
}

void Server::on_connection_shutdown(gabriel::base::Client_Connection *client_connection)
{
}

bool Server::on_connection_shutdown(gabriel::base::Server_Connection *server_connection)
{
    return Super::on_connection_shutdown(server_connection);
}
    
bool Server::verify_connection(gabriel::base::Client_Connection *client_connection)
{
    return true;
}

void Server::update_hook()
{
}

void Server::do_reconnect()
{
    Super::do_reconnect();
}
    
void Server::register_msg_handler()
{
    using namespace gabriel::protocol::server;
    using namespace std::placeholders;
    Super::register_msg_handler();
    m_server_msg_handler.register_handler(DEFAULT_MSG_TYPE, REGISTER_ORDINARY_SERVER, std::bind(&Server::register_rsp_from, this, _1, _2, _3));
}

void Server::do_main_on_server_connection()
{
    Super::do_main_on_server_connection();
}
    
bool Server::init_hook()
{
    lua_State *state = luaL_newstate();
    luaL_openlibs(state);
    register_lua2cpp(state);
    
    if(luaL_dofile(state, "script/gabriel/script/main.lua") != 0)
    {
        cout << "lua error: " << lua_tostring(state, -1) << endl;
        
        return false;
    }
    
    m_main_lua_state = state;
    
    return Super::init_hook();
}

void Server::init_reactor()
{
    delete ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(100, true), true), true);
}

void Server::register_rsp_from(gabriel::base::Connection *connection, void *data, uint32 size)
{
    using namespace gabriel::protocol::server;    
    PARSE_FROM_ARRAY(Register_Ordinary_Rsp, msg, data, size);
    
    if(id() > 0)
    {
        return;
    }
    
    if(msg.info_size() < 2)
    {
        state(gabriel::base::SERVER_STATE::SHUTDOWN);
        cout << "error: information of this server received from center server is wrong" << endl;
        
        return;
    }
    
    for(int32 i = 0; i != msg.info_size(); ++i)
    {
        const auto &info = msg.info(i);

        if(info.server_type() == gabriel::base::GAME_SERVER)
        {
            id(info.server_id());
                
            if(m_acceptor.open(ACE_INET_Addr(info.port(), info.inner_addr().c_str()), ACE_Reactor::instance()) < 0)
            {
                state(gabriel::base::SERVER_STATE::SHUTDOWN);
                cout << "error: start game server (id=" << id() << ") failed" << endl;
            
                return;
            }

            cout << "start game server (id=" << id() << ") ok" << endl;
            set_proc_name_and_log_dir("gabriel_game_server___%u___%u", zone_id(), id());
        }
        else
        {
            gabriel::base::Server_Connection *tmp = &m_record_connection;

            if(m_connector.connect(tmp, ACE_INET_Addr(info.port(), info.inner_addr().c_str())) < 0)
            {
                cout << "error: connect to record server failed" << endl;
                state(gabriel::base::SERVER_STATE::SHUTDOWN);
            }
            else
            {
                cout << "connect to record server ok" << endl;
            }
        }
    }
}
    
void Server::fini_hook()
{
}

}
}

#include "gabriel/main.cpp"
