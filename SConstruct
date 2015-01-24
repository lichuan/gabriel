import os
import copy
SetOption("random", 1)

def get_static_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-2]

def get_shared_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-3]

env = Environment(CCFLAGS='-g -Wall -Werror -std=c++11', CPPPATH=[
        "#src",
        "#3rd-party/ACE_wrappers",
        "#3rd-party/yaml-cpp/include",
        "#protocol/generated",
        "/usr/include/mysql",
        "/usr/include/mysql++",
        "/usr/include/lua5.2"
        ])

#base lib
Export("env")
gabriel_base_lib = SConscript("src/gabriel/base/SConscript", variant_dir="build/base", duplicate=0)

#protocol lib
gabriel_protocol_lib = SConscript("protocol/SConscript", variant_dir="build/protocol", duplicate=0)

libs = [
  get_static_library_name(gabriel_base_lib),
  get_static_library_name(gabriel_protocol_lib),
  "ACE",
  "protobuf",
  "pthread",
  "mysqlpp",
  "lua5.2",
  "yaml-cpp"
]

lib_path = [
  "#build/base",
  "#build/protocol",
  "#3rd-party/yaml-cpp/lib",
  os.environ["ACE_ROOT"] + "/lib",
  "/usr/lib/x86_64-linux-gnu"
]

env.Replace(LIBS=libs, LIBPATH=lib_path)

#supercenter
gabriel_supercenter_server = SConscript("src/gabriel/supercenter/SConscript", variant_dir="build/supercenter", duplicate=0)
env.Install("build/bin", gabriel_supercenter_server)

#superrecord
gabriel_superrecord_server = SConscript("src/gabriel/superrecord/SConscript", variant_dir="build/superrecord", duplicate=0)
env.Install("build/bin", gabriel_superrecord_server)

#center
gabriel_center_server = SConscript("src/gabriel/center/SConscript", variant_dir="build/center", duplicate=0)
env.Install("build/bin", gabriel_center_server)

#record
gabriel_record_server = SConscript("src/gabriel/record/SConscript", variant_dir="build/record", duplicate=0)
env.Install("build/bin", gabriel_record_server)

#login
gabriel_login_server = SConscript("src/gabriel/login/SConscript", variant_dir="build/login", duplicate=0)
env.Install("build/bin", gabriel_login_server)

#game
game_cpppath = copy.deepcopy(env["CPPPATH"])
game_cpppath.append("#src/gabriel/game/lua2cpp")
game_env = env.Clone(CPPPATH=game_cpppath)
Export("game_env")
gabriel_game_server = SConscript("src/gabriel/game/SConscript", variant_dir="build/game", duplicate=0)
env.Install("build/bin", gabriel_game_server)

#gateway
gabriel_gateway_server = SConscript("src/gabriel/gateway/SConscript", variant_dir="build/gateway", duplicate=0)
env.Install("build/bin", gabriel_gateway_server)


