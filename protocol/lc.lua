#!/usr/bin/env lua


package.cpath = "./protobuf/?.so"
package.path = "./generated/?.lua;./protobuf/?.lua;"
require("gabriel.protocol.server.db_pb")
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

function tst(data)
   local msg = gabriel.protocol.server.db_pb.DB_Msg()
   msg:ParseFromString(data)
   print("begin............................")
   print(msg)
   print(string.len(data))
   print("end..............................")
   return st
end




