package.path = "script/?.lua;script/protobuf/?.lua"
package.cpath = "script/protobuf/?.so"
require "gabriel.protocol.client.skill_pb"
require "gabriel.script.npc.kill"

print()
print("call function in subdir")
gabriel.script.npc.kill.killnpc()

print()
print("dump protobuf message:")
print()

local msg = gabriel.protocol.client.skill_pb.SkillTree()
msg.treeid = 882
msg.skillarray:add().skillid = 23
msg.skillarray:add().skillname = "2223333"
msg.skillarray:add().skillid = 44432
msg.pubmsg.id = 838
msg.pubmsg.name = "pubbbbbbbbb"
msg.pubenum = gabriel.protocol.lcjj_pb.Pub_3
msg.pubmsgarray:add().id = 9
msg.pubmsgarray:add().name = "eeee"
msg.pubmsgarray:add().id = 232
msg.pubmsgarray:add().name ="abd"

print(msg)



