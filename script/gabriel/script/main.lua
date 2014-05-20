package.path = "script/?.lua"
require "gabriel.script.skill.cast"

function test()
    print("test..................")
end

print("start lua..................")

gabriel.script.skill.cast.createskill()

local obj = _gabriel._supercenter.Server.new()
print(obj)
