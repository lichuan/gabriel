import os
SetOption("random", 1)

def get_static_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-2]

def get_shared_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-3]

env = Environment(CCFLAGS='-g', CPPPATH=["#src", "#src/3rd-party/ACE_wrappers", "#src/3rd-party/protobuf/src"])

#base
Export("env")
gabriel_base = SConscript("src/gabriel/base/SConscript", variant_dir="build/base", duplicate=0)

#protocol
gabriel_protocol = SConscript("src/gabriel/protocol/SConscript", variant_dir="build/protocol", duplicate=0)

libs = [
  get_static_library_name(gabriel_base),
  get_static_library_name(gabriel_protocol)
]

lib_path = [
  "#build/base",
  "#build/protocol"
]

#game
env.Replace(LIBS=libs, LIBPATH=lib_path)
gabriel_game = SConscript("src/gabriel/game/SConscript", variant_dir="build/game", duplicate=0)
env.Install("build/bin", gabriel_game)
