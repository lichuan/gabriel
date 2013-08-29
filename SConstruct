import os
SetOption("random", 1)

def get_static_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-2]

def get_shared_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-3]

env = Environment(CCFLAGS='-g', CPPPATH=["#src/gabriel", "#src/3rd-party/ACE_wrappers"])

#base
Export("env")
gabriel_base = SConscript("src/gabriel/base/SConscript", variant_dir="build/base", duplicate=0)

#game
env.Replace(LIBS=get_static_library_name(gabriel_base), LIBPATH="#build/base")
gabriel_game = SConscript("src/gabriel/game/SConscript", variant_dir="build/game", duplicate=0)
env.Install("build/bin", gabriel_game)
