import os
SetOption("random", 1)

def get_static_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-2]

def get_shared_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-3]

env = Environment(CCFLAGS='-g -std=c++11', CPPPATH=[
        "#protocol/generated",
        "/usr/include/lua5.2"
        ],
        LINKFLAGS='-ldl')        
Export("env")
#protocol lib
gabriel_protocol_lib = SConscript("protocol/SConscript", variant_dir="build/protocol", duplicate=0)

libs = [
  get_static_library_name(gabriel_protocol_lib),
  "protobuf",
  "pthread",
  "lua5.2"
]

lib_path = [
  "#build/protocol",
  "/usr/lib/x86_64-linux-gnu/"
]

env.Replace(LIBS=libs, LIBPATH=lib_path)
env.Program("lc.cpp")