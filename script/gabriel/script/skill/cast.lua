require "gabriel.script.npc.kill"
module(..., package.seeall)
function createskill()
    print("createskillllllllllllllllllllllllll")
    local npc = gabriel.script.npc.kill
    npc.killnpc()
    gabriel.script.npc.kill.killnpc()
end

print("start cast.lua")
