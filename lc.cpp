#include <iostream>
#include <string>
#include "lua.hpp"
using namespace std;

#include "gabriel/protocol/server/db.pb.h"

/*
local db = gabriel.protocol.server.db_pb
local msg = db.DB_Msg()
msg.dbmsg_id = 98
msg.monster.monster_id = 22
msg.monster.monster_hp =92
msg.monster.monster_name = "a huge monster"
msg.outerenum = db.ENUM_OUTER_1
msg.outer.outer_id = 3332
msg.outer.outer_string = "outer string"
msg.innerenum = msg.outer.ENUM_INNER_1
msg.inner.inner_id = 892
msg.inner.inner_string = "inner string"
msg.pubmsg.pubid = 232
msg.pubmsg.pubname = "pub name"
msg.pubenum = gabriel.protocol.shared_pb.ENUMPUB_1
msg.pubouter.outerid = 1112
msg.pubouter.outername = "outername"
msg.pubinner.innerid = 29323
msg.pubinner.innername = "innername"
msg.pubinnerenum = msg.pubouter.ENUM_PUBINNER_2
local st = msg:SerializeToString()
print(msg)
print(string.len(st))
*/

int main()
{
    gabriel::protocol::server::DB_Msg msg;
    msg.set_dbmsg_id(98);
    msg.mutable_monster()->set_monster_id(22);
    msg.mutable_monster()->set_monster_hp(92);
    msg.mutable_monster()->set_monster_name("a huge monster");
    msg.set_outerenum(gabriel::protocol::server::ENUM_OUTER_1);
    msg.mutable_outer()->set_outer_id(3332);
    msg.mutable_outer()->set_outer_string("outer string");
    msg.set_innerenum(gabriel::protocol::server::Outer_InnerEnum_ENUM_INNER_1);
    msg.mutable_inner()->set_inner_id(892);
    msg.mutable_inner()->set_inner_string("inner string");
    msg.mutable_pubmsg()->set_pubid(232);
    msg.mutable_pubmsg()->set_pubname("pub name");
    msg.set_pubenum(gabriel::protocol::ENUMPUB_1);
    msg.mutable_pubouter()->set_outerid(1112);
    msg.mutable_pubouter()->set_outername("outername");
    msg.mutable_pubinner()->set_innerid(29323);
    msg.mutable_pubinner()->set_innername("innername");
    msg.set_pubinnerenum(gabriel::protocol::PubOuter_PubInnerEnum_ENUM_PUBINNER_2);
    std::string data;    
    msg.SerializeToString(&data);
    cout << "c++ len: " << msg.ByteSize() << endl;    
    lua_State *state = luaL_newstate();    
    luaL_openlibs(state);
    
    if(luaL_dofile(state, "lc.lua") != 0)
    {
        cout << "err: " << lua_tostring(state, -1) << endl;
    }
    
    lua_getglobal(state, "tst");
    lua_pushstring(state, data.c_str());

    if(lua_pcall(state, 1, 1, 0) != 0) {
        cout << "err: " << lua_tostring(state, -1) << endl;
    }

    string rets = lua_tostring(state, -1);
    cout << "size: " << rets.size() << endl;
    gabriel::protocol::server::DB_Msg msg2;
    msg2.ParseFromString(rets);
    cout << "---------------------------------------------------msg2-------------------------------------" << endl;
    cout << msg2.DebugString() << endl;
    cout << "main end" << endl;    
}
