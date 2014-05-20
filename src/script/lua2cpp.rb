#!/usr/bin/env ruby

$decl = <<-INTRO
author: lichuan
qq: 308831759
email: 308831759@qq.com
homepage: www.lichuan.me
github: https://github.com/lichuan/lua2cpp
date: 2013-05-11
desc: this is the generator of binding code between lua and c++
INTRO

def parse_namespace_class(tbl_namespace, tbl_class, full_name)
  full_name.prepend(".")
  full_name.scan(/\._([^\.]+)/) do |arr|
    namespace = arr[0]
    tbl_namespace[tbl_namespace.size] = namespace
  end
  full_name.scan(/\.([^_][^\.]*)/) do |arr|
    class_name = arr[0]
    tbl_class[tbl_class.size] = class_name
  end
end

def is_basic_type(type_str)
  ["string", "int32", "number", "uint32", "bool"].each { |basic_type| return true if type_str == basic_type }
  false
end

def parse_argument(tbl_arg, args)
  args.scan(/([^\s,]+)/) do |arr|
    arg = arr[0]
    idx = tbl_arg.size
    tbl_arg[idx] = {}
    parse_type(tbl_arg[idx], arg, false)
  end
end

def error_msg(str)
  puts "Error: #{str}"
  exit(1)
end

def parse_function_1(tbl_func, func_str)
  match_list = /static (.*)/.match(func_str)
  if match_list.nil?
    parse_function_2(tbl_func, func_str)
  else
    tbl_func["is_static"] = true
    parse_function_2(tbl_func, match_list[1])
  end
end

def parse_type(tbl_type, type_str, is_ret)
  return if type_str.empty?
  match_list = /([^\s\*\|\&]+)(\*|\&)?(\|gc\|)?/.match(type_str)
  if match_list.nil?
    error_msg("type invalid: #{type_str}")
  else
    if match_list[3] == "|gc|"
      if not is_ret
        error_msg("function parameter should not be |gc|: #{type_str}")
      end
    end
    type = match_list[1]
    tbl_type["name"] = type
    if is_basic_type type
      tbl_type["is_basic"] = true
      ["*", "|gc|", "&"].each do |v|
        error_msg("basic type does not support #{v}: #{type_str}") if match_list[2] == v or match_list[3] == v
      end
    elsif not $reg_info.has_key?(match_list[1])
      error_msg("type #{match_list[1]} is not registered: #{type_str}")
    else
      if match_list[2] == "*"
        tbl_type["is_ptr"] = true
      elsif match_list[2] == "&"
        tbl_type["is_ref"] = true
      end
      if match_list[3] == "|gc|"
        tbl_type["is_gc"] = true
      end
    end
  end
end

def parse_function_2(tbl_func, func_str)
  match_list = /([^\s]*?) *([^\s]+)\((.*)\)/.match(func_str)
  tbl_func["ret_type"] = {}
  if match_list.nil?
    parse_function_3(tbl_func, func_str)
  else
    func_name = match_list[2]
    if not func_name.empty?
      tbl_func["name"] = func_name
      if not tbl_func.has_key?("export_name")
        tbl_func["export_name"] = func_name
      end
      if not match_list[1].empty?
        parse_type(tbl_func["ret_type"], match_list[1], true)
      end
      parse_argument(tbl_func["arg"], match_list[3])
    end
  end
end

def parse_function_3(tbl_func, func_str)
  match_list = /(^[^\s=]*?) *([^\s=]+)=([^\s]+)/.match(func_str)
  if match_list.nil?
    match_list = /(^[^\s=]+) +([^\s]+)/.match(func_str)
    if match_list.nil?
      error_msg("function is invalid: #{func_str}")
    else
      parse_type(tbl_func["ret_type"], match_list[1], true)
      tbl_func["is_get"] = true
      tbl_func["name"] = match_list[2]
      if not tbl_func.has_key?("export_name")
        tbl_func["export_name"] = match_list[2]
      end
    end
  elsif not tbl_func.has_key?("export_name")
    error_msg("no export name assign: #{func_str}")
  else
    tbl_func["is_set"] = true
    tbl_func["name"] = match_list[2]
    parse_argument(tbl_func["arg"], match_list[3])
    parse_type(tbl_func["ret_type"], match_list[1], true)
  end
end

def parse_function(tbl_func, func_str)
  match_list = /([^\s]+) : (.+)/.match(func_str)
  if match_list.nil?
    parse_function_1(tbl_func, func_str)
  else
    tbl_func["export_name"] = match_list[1]
    parse_function_1(tbl_func, match_list[2])
  end
end

def parse_file
  $reg_info = {}
  lua_reg_content = File.open("./lua2cpp.txt").readlines().join
  match_list = /([^\{]*?)[^\n]*\n\{/.match(lua_reg_content)
  $head = match_list[1]
  body = lua_reg_content[$head.size..-1]
  body.scan(/([^\n]+)\n\{\n(.*?)\n\}/m) do |part1, part2|
    full_name = part1.match(/[^\s]*/)[0]
    tbl = {}
    tbl["namespace"] = {}
    tbl["class"] = {}
    tbl["name"] = full_name
    tbl["super"] = {}
    tbl["function"] = {}
    error_msg("#{part1} have already registered") if $reg_info.has_key?(full_name)
    $reg_info[full_name] = tbl
  end
  body.scan(/(^[^\s][^\n]*)\n\{\n(.*?)\n\}/m) do |part1, part2|
    full_name = part1.match(/[^\s]*/)[0]
    tbl = $reg_info[full_name]
    parse_namespace_class(tbl["namespace"], tbl["class"], full_name)
    if not tbl["class"].empty?
      match_list = / extends (.*)/.match(part1)
      if not match_list.nil?
        super_class_list = match_list[1]
        super_class_list.scan(/([^\s,]+)/) do |arr|
          super_class = arr[0]
          error_msg("#{super_class} is not registered: #{part1}") if not $reg_info.has_key?(super_class)
          tbl["super"][tbl["super"].size] = super_class
        end
      end
    end
    func_idx = 0
    new_idx = 0
    part2.scan(/([^\s].+)/) do |arr|
      function_line = arr[0]
      tbl["function"][func_idx] = {}
      tbl["function"][func_idx]["arg"] = {}
      match_list = /^\((.*)\)/.match(function_line)
      if not match_list.nil?
        tbl["function"][func_idx]["is_new"] = true
        tbl["function"][func_idx]["export_name"] = "new" if new_idx == 0
        tbl["function"][func_idx]["export_name"] = "new#{new_idx}" if new_idx > 0
        args = match_list[1]
        parse_argument(tbl["function"][func_idx]["arg"], args)
        new_idx += 1
      else
        parse_function(tbl["function"][func_idx], function_line)
      end
      func_idx += 1
    end
  end
end

def generate_header()
  header = "/*\n"
  $decl.each_line do |line|
    header += " #{line}"
  end
  header = header.sub(/this is the generator of binding code between lua and c\+\+/, 'this is the binding code between lua and c++ generated by lua2cpp.rb')
  header += "*/

"
  #header += $head
  header += <<-HEADER
static void get_global_table(lua_State *lua_state, const char *nodes_name)
{
    char buf[1024];
    strcpy(buf, nodes_name);
    char *p = buf;
    const char *q = p;
    int count = 0;

    while(*p != 0)
    {
        if(*p == '.')
        {
            *p = 0;

            if(count == 0)
            {
                lua_getglobal(lua_state, q);

                if(lua_isnil(lua_state, -1))
                {
                    return;
                }
            }
            else
            {
                lua_pushstring(lua_state, q);
                lua_rawget(lua_state, -2);

                if(lua_isnil(lua_state, -1))
                {
                    return;
                }
            }

            q = p + 1;
            ++count;
        }

        ++p;
    }
    
    if(count == 0)
    {
        lua_getglobal(lua_state, q);

        if(lua_isnil(lua_state, -1))
        {
            return;
        }
    }
    else
    {
        lua_pushstring(lua_state, q);
        lua_rawget(lua_state, -2);

        if(lua_isnil(lua_state, -1))
        {
            return;
        }
    }
}

static void build_global_table(lua_State *lua_state, const char *nodes_name)
{
    char buf[1024];
    strcpy(buf, nodes_name);
    char *p = buf;
    const char *q = p;
    int count = 0;

    while(*p != 0)
    {
        if(*p == '.')
        {
            *p = 0;

            if(count == 0)
            {
                lua_getglobal(lua_state, q);

                if(lua_isnil(lua_state, -1))
                {
                    lua_newtable(lua_state);
                    lua_pushvalue(lua_state, -1);
                    lua_setglobal(lua_state, q);
                }
            }
            else
            {
                lua_pushstring(lua_state, q);
                lua_rawget(lua_state, -2);

                if(lua_isnil(lua_state, -1))
                {
                    lua_pop(lua_state,  1);
                    lua_pushstring(lua_state, q);
                    lua_newtable(lua_state);
                    lua_pushvalue(lua_state, -1);
                    lua_insert(lua_state, -4);
                    lua_rawset(lua_state, -3);
                    lua_pop(lua_state, 1);
                }
            }

            q = p + 1;
            ++count;
        }

        ++p;
    }
    
    if(count == 0)
    {
        lua_getglobal(lua_state, q);

        if(lua_isnil(lua_state, -1))
        {
            lua_newtable(lua_state);
            lua_setglobal(lua_state, q);
        }
    }
    else
    {
        lua_pushstring(lua_state, q);
        lua_rawget(lua_state, -2);

        if(lua_isnil(lua_state, -1))
        {
            lua_pop(lua_state,  1);
            lua_pushstring(lua_state, q);
            lua_newtable(lua_state);
            lua_rawset(lua_state, -3);
        }
    }

    lua_settop(lua_state, 0);
}
HEADER
end

def generate_ns_class_name(ns_tbl, cls_tbl)
  gen_str = ""
  ns_tbl.each_value do |ns|
    gen_str << ns << "::"
  end
  have_cls = false
  cls_tbl.each_value do |cls|
    have_cls = true
    gen_str << cls << "::"
  end
  gen_str = gen_str[0..-3] if have_cls
  gen_str
end

def generate_ns_class_prefix(ns_tbl, cls_tbl)
  gen_str = ""
  ns_tbl.each_value do |ns|
    gen_str << ns << "::"
  end
  cls_tbl.each_value do |cls|
    gen_str << cls << "::"
  end
  gen_str
end

def generate_new_function(tbl, func)
  ns_cls_name = generate_ns_class_name(tbl["namespace"], tbl["class"])
  gen_list = generate_arg_list func, 1
  gen_str = gen_list[0]
  func_str = gen_list[1]
  gen_str += %Q{
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = 1; /* need gc default in constructor */
    udata += 1;
    *(#{ns_cls_name}**)udata = new #{ns_cls_name}(#{func_str.join(', ')});
    luaL_setmetatable(lua_state, "#{tbl["name"]}");

    return 1;
}
  gen_str
end

def generate_function(tbl, func)
  gen_str = ""
  ns_cls_prefix = generate_ns_class_prefix(tbl["namespace"], tbl["class"])
  ns_cls_name = generate_ns_class_name(tbl["namespace"], tbl["class"])
  if func.has_key? "is_static" or tbl["class"].empty?
    gen_list = generate_arg_list func, 1
    gen_str += gen_list[0]
    func_str = gen_list[1]
    if func["ret_type"].empty?
       gen_str += "
    #{ns_cls_prefix}#{func["name"]}(#{func_str.join(', ')});

    return 0;
"
    elsif func["ret_type"].has_key? "is_basic"
      case func["ret_type"]["name"]
      when "bool"
        gen_str += "
    bool v = #{ns_cls_prefix}#{func["name"]}(#{func_str.join(', ')});
    lua_pushboolean(lua_state, v ? 1 : 0);

    return 1;
"
      when "int32"
        gen_str += "
    int32 v = #{ns_cls_prefix}#{func["name"]}(#{func_str.join(', ')});
    lua_pushinteger(lua_state, v);

    return 1;
"
      when "uint32"
        gen_str += "
    uint32 v = #{ns_cls_prefix}#{func["name"]}(#{func_str.join(', ')});
    lua_pushunsigned(lua_state, v);

    return 1;
"
      when "number"
        gen_str += "
    double v = #{ns_cls_prefix}#{func["name"]}(#{func_str.join(', ')});
    lua_pushnumber(lua_state, v);

    return 1;
"
      when "string"
        gen_str += "
    std::string v = #{ns_cls_prefix}#{func["name"]}(#{func_str.join(', ')});
    lua_pushstring(lua_state, v.c_str());

    return 1;
"
      else
      end
    elsif func["ret_type"].has_key? "is_ptr"
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    const #{ns_cls_name} *v = #{ns_cls_prefix}#{func["name"]}(#{func_str.join(', ')});
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = #{func["ret_type"].has_key?("is_gc") ? 1 : 0};
    udata += 1;
    *(#{ns_cls_name}**)udata = (#{ns_cls_name}*)v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    elsif func["ret_type"].has_key? "is_ref"
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    const #{ns_cls_name} *v = &#{ns_cls_prefix}#{func["name"]}(#{func_str.join(', ')});
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = #{func["ret_type"].has_key?("is_gc") ? 1 : 0};
    udata += 1;
    *(#{ns_cls_name}**)udata = (#{ns_cls_name}*)v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    else
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    #{ns_cls_name} *v = new #{ns_cls_name};
    *v = #{ns_cls_prefix}#{func["name"]}(#{func_str.join(', ')});
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = 1; /* no ptr, no ref, it's a new obj, so it need gc */
    udata += 1;
    *(#{ns_cls_name}**)udata = v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    end
  else #non-static
    gen_str += %Q{
    uint32 *udata_self = (uint32*)luaL_checkudata(lua_state, 1, "#{tbl["name"]}");
    udata_self += 1;
    #{ns_cls_name} *obj = *(#{ns_cls_name}**)udata_self;}
    gen_list = generate_arg_list func, 2
    gen_str += gen_list[0]
    func_str = gen_list[1]
    if func["ret_type"].empty?
      gen_str += "
    obj->#{func["name"]}(#{func_str.join(', ')});

    return 0;
"
    elsif func["ret_type"].has_key? "is_basic"
      case func["ret_type"]["name"]
      when "bool"
        gen_str += "
    bool v = obj->#{func["name"]}(#{func_str.join(', ')});
    lua_pushboolean(lua_state, v ? 1 : 0);

    return 1;
"
      when "int32"
        gen_str += "
    int32 v = obj->#{func["name"]}(#{func_str.join(', ')});
    lua_pushinteger(lua_state, v);

    return 1;
"
      when "uint32"
        gen_str += "
    uint32 v = obj->#{func["name"]}(#{func_str.join(', ')});
    lua_pushunsigned(lua_state, v);

    return 1;
"
      when "number"
        gen_str += "
    double v = obj->#{func["name"]}(#{func_str.join(', ')});
    lua_pushnumber(lua_state, v);

    return 1;
"
      when "string"
        gen_str += "
    std::string v = obj->#{func["name"]}(#{func_str.join(', ')});
    lua_pushstring(lua_state, v.c_str());

    return 1;
"
      else
      end
    elsif func["ret_type"].has_key? "is_ptr"
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    const #{ns_cls_name} *v = obj->#{func["name"]}(#{func_str.join(', ')});
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = #{func["ret_type"].has_key?("is_gc") ? 1 : 0};
    udata += 1;
    *(#{ns_cls_name}**)udata = (#{ns_cls_name}*)v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    elsif func["ret_type"].has_key? "is_ref"
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    const #{ns_cls_name} *v = &obj->#{func["name"]}(#{func_str.join(', ')});
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = #{func["ret_type"].has_key?("is_gc") ? 1 : 0};
    udata += 1;
    *(#{ns_cls_name}**)udata = (#{ns_cls_name}*)v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    else
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    #{ns_cls_name} *v = new #{ns_cls_name};
    *v = obj->#{func["name"]}(#{func_str.join(', ')});
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = 1; /* no ptr, no ref, it's a new obj, so it need gc */
    udata += 1;
    *(#{ns_cls_name}**)udata = v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    end
  end
  gen_str
end

def generate_get_function(tbl, func)
  ns_cls_prefix = generate_ns_class_prefix(tbl["namespace"], tbl["class"])
  ns_cls_name = generate_ns_class_name(tbl["namespace"], tbl["class"])
  gen_str = ""  
  if func.has_key? "is_static" or tbl["class"].empty?
    if func["ret_type"].has_key? "is_basic"
      case func["ret_type"]["name"]
      when "bool"
        gen_str += "
    bool v = #{ns_cls_prefix}#{func["name"]};
    lua_pushboolean(lua_state, v ? 1 : 0);

    return 1;
"
      when "int32"
        gen_str += "
    int32 v = #{ns_cls_prefix}#{func["name"]};
    lua_pushinteger(lua_state, v);

    return 1;
"
      when "uint32"
        gen_str += "
    uint32 v = #{ns_cls_prefix}#{func["name"]};
    lua_pushunsigned(lua_state, v);

    return 1;
"
      when "number"
        gen_str += "
    double v = #{ns_cls_prefix}#{func["name"]};
    lua_pushnumber(lua_state, v);

    return 1;
"
      when "string"
        gen_str += "
    std::string v = #{ns_cls_prefix}#{func["name"]};
    lua_pushstring(lua_state, v.c_str());

    return 1;
"
      else
      end
    elsif func["ret_type"].has_key? "is_ptr"
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    const #{ns_cls_name} *v = #{ns_cls_prefix}#{func["name"]};
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = #{func["ret_type"].has_key?("is_gc") ? 1 : 0};
    udata += 1;
    *(#{ns_cls_name}**)udata = (#{ns_cls_name}*)v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    elsif func["ret_type"].has_key? "is_ref"
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    const #{ns_cls_name} *v = &#{ns_cls_prefix}#{func["name"]};
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = #{func["ret_type"].has_key?("is_gc") ? 1 : 0};
    udata += 1;
    *(#{ns_cls_name}**)udata = (#{ns_cls_name}*)v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    else
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    #{ns_cls_name} *v = new #{ns_cls_name};
    *v = #{ns_cls_prefix}#{func["name"]};
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = 1; /* no ptr, no ref, it's a new obj, so it need gc */
    udata += 1;
    *(#{ns_cls_name}**)udata = v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    end
  else #non-static
    gen_str += %Q{
    uint32 *udata_self = (uint32*)luaL_checkudata(lua_state, 1, "#{tbl["name"]}");
    udata_self += 1;
    #{ns_cls_name} *obj = *(#{ns_cls_name}**)udata_self;}
    if func["ret_type"].has_key? "is_basic"
      case func["ret_type"]["name"]
      when "bool"
        gen_str += "
    bool v = obj->#{func["name"]};
    lua_pushboolean(lua_state, v ? 1 : 0);

    return 1;
"
      when "int32"
        gen_str += "
    int32 v = obj->#{func["name"]};
    lua_pushinteger(lua_state, v);

    return 1;
"
      when "uint32"
        gen_str += "
    uint32 v = obj->#{func["name"]};
    lua_pushunsigned(lua_state, v);

    return 1;
"
      when "number"
        gen_str += "
    double v = obj->#{func["name"]};
    lua_pushnumber(lua_state, v);

    return 1;
"
      when "string"
        gen_str += "
    std::string v = obj->#{func["name"]};
    lua_pushstring(lua_state, v.c_str());

    return 1;
"
      else
      end
    elsif func["ret_type"].has_key? "is_ptr"
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    const #{ns_cls_name} *v = obj->#{func["name"]};
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = #{func["ret_type"].has_key?("is_gc") ? 1 : 0};
    udata += 1;
    *(#{ns_cls_name}**)udata = (#{ns_cls_name}*)v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    elsif func["ret_type"].has_key? "is_ref"
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    const #{ns_cls_name} *v = &obj->#{func["name"]};
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = #{func["ret_type"].has_key?("is_gc") ? 1 : 0};
    udata += 1;
    *(#{ns_cls_name}**)udata = (#{ns_cls_name}*)v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    else
      ret_type_info = $reg_info[func["ret_type"]["name"]]
      ns_cls_name = generate_ns_class_name(ret_type_info["namespace"], ret_type_info["class"])
      gen_str += %Q{
    #{ns_cls_name} *v = new #{ns_cls_name};
    *v = obj->#{func["name"]};
    uint32 *udata = (uint32*)lua_newuserdata(lua_state, sizeof(uint32) + sizeof(#{ns_cls_name}*));
    uint32 &gc_flag = *udata;
    gc_flag = 1; /* no ptr, no ref, it's a new obj, so it need gc */
    udata += 1;
    *(#{ns_cls_name}**)udata = v;
    luaL_setmetatable(lua_state, "#{ret_type_info["name"]}");

    return 1;
}
    end
  end
  gen_str
end

def generate_arg_list(func, start_idx)
  gen_str = ""
  idx, stack_idx = 1, start_idx
  func_str = []
  func["arg"].each_value do |arg|
    if arg.has_key? "is_basic"
      case arg["name"]
      when "bool"
        gen_str += "
    bool arg_#{idx} = luaL_checkint(lua_state, #{stack_idx}) > 0 ? true : false;"
        func_str << "arg_#{idx}"
      when "int32"
        gen_str += "
    int32 arg_#{idx} = luaL_checkint(lua_state, #{stack_idx});"
        func_str << "arg_#{idx}"
      when "number"
        gen_str += "
    double arg_#{idx} = luaL_checknumber(lua_state, #{stack_idx});"
        func_str << "arg_#{idx}"
      when "uint32"
        gen_str += "
    uint32 arg_#{idx} = luaL_checkunsigned(lua_state, #{stack_idx});"
        func_str << "arg_#{idx}"
      when "string"
        gen_str += "
    const char *arg_#{idx} = luaL_checkstring(lua_state, #{stack_idx});"
        func_str << "arg_#{idx}"
      else        
      end
    elsif arg.has_key? "is_ptr"
      arg_cls_info = $reg_info[arg["name"]]
      arg_cls = generate_ns_class_name(arg_cls_info["namespace"], arg_cls_info["class"])
      gen_str += %Q{
    uint32 *udata_#{idx} = (uint32*)luaL_checkudata(lua_state, #{stack_idx}, "#{arg_cls_info["name"]}");
    udata_#{idx} += 1;
    #{arg_cls} *arg_#{idx} = *(#{arg_cls}**)udata_#{idx};}
      func_str << "arg_#{idx}"
    else #is_ref
      arg_cls_info = $reg_info[arg["name"]]
      arg_cls = generate_ns_class_name(arg_cls_info["namespace"], arg_cls_info["class"])
      gen_str += %Q{
    uint32 *udata_#{idx} = (uint32*)luaL_checkudata(lua_state, #{stack_idx}, "#{arg_cls_info["name"]}");
    udata_#{idx} += 1;
    #{arg_cls} *arg_#{idx} = *(#{arg_cls}**)udata_#{idx};}
      func_str << "*arg_#{idx}"
    end
    idx += 1
    stack_idx += 1
  end
  gen_str += "
    lua_settop(lua_state, 0);"
  [gen_str, func_str]
end

def generate_set_function(tbl, func)
  gen_str = ""
  ns_cls_prefix = generate_ns_class_prefix(tbl["namespace"], tbl["class"])
  ns_cls_name = generate_ns_class_name(tbl["namespace"], tbl["class"])
  if func.has_key? "is_static" or tbl["class"].empty?
    gen_list = generate_arg_list func, 1
    gen_str += gen_list[0]
    func_str = gen_list[1]
    gen_str += "
    #{ns_cls_prefix}#{func["name"]} = #{func_str.join(', ')};

    return 0;
"
  else
    gen_str += %Q{
    uint32 *udata = (uint32*)luaL_checkudata(lua_state, 1, "#{tbl["name"]}");
    udata += 1;
    #{ns_cls_name} *obj = *(#{ns_cls_name}**)udata;}
    gen_list = generate_arg_list func, 2
    gen_str += gen_list[0]
    func_str = gen_list[1]
    gen_str += "
    obj->#{func["name"]} = #{func_str.join(', ')};

    return 0;
"
  end
  gen_str
end

def merge_super_function_to_derived()
  $reg_info.each_value do |v|
    v["super"].each_value do |super_cls|
      merge_super_function_to_derived_impl(v["function"], $reg_info[super_cls])
    end
  end
end

def merge_super_function_to_derived_impl(derived_func_tbl, super_info)
  super_info["super"].each_value do |super_cls|
    merge_super_function_to_derived_impl(super_info["function"], $reg_info[super_cls])
  end
  already_exist_functions = []
  derived_func_tbl.each_value do |func|
    next if func.has_key? "is_static" or func.has_key? "is_new"
    already_exist_functions << func["export_name"]
  end
  super_info["function"].each_value do |func|
    next if func.has_key? "is_static" or func.has_key? "is_new"
    next if already_exist_functions.include? func["export_name"]
    derived_func_tbl[derived_func_tbl.size] = func
  end
end

def generate_file()
  $reg_func_tbl = {}
  gen_str = generate_header
  merge_super_function_to_derived
  $reg_info.each do |full_name, tbl|
    $reg_func_tbl[full_name] = []
    tbl["function"].each_value do |func|
      gen_str += "
static int "
      gen_func_name = "lua"
      tbl["namespace"].each_value do |ns|
        gen_func_name += "____" + ns
      end
      tbl["class"].each_value do |cls|
        gen_func_name += "___" + cls
      end
      gen_func_name += "__" + func["export_name"]
      $reg_func_tbl[full_name].push([func["export_name"], gen_func_name])
      gen_str += gen_func_name + "(lua_State *lua_state)"
      gen_str += "
{"
      if func.has_key? "is_new"
        gen_str += generate_new_function(tbl, func)
      elsif func.has_key? "is_get"
        gen_str += generate_get_function(tbl, func)
      elsif func.has_key? "is_set"
        gen_str += generate_set_function(tbl, func)
      else
        gen_str += generate_function(tbl, func)
      end
      gen_str += "}
"
    end
    if not tbl["class"].empty? #generate gc function
      gen_str += "
static int "
      gen_func_name = "lua"
      tbl["namespace"].each_value do |ns|
        gen_func_name += "____" + ns
      end
      tbl["class"].each_value do |cls|
        gen_func_name += "___" + cls
      end
      gen_func_name += "__" + "garbage_colloect"
      $reg_func_tbl[full_name].push(["__gc", gen_func_name])
      gen_str += gen_func_name + "(lua_State *lua_state)"
      gen_str += "
{"
      ns_cls_name = generate_ns_class_name(tbl["namespace"], tbl["class"])
      gen_str += %Q{
    uint32 *udata = (uint32*)luaL_checkudata(lua_state, 1, "#{tbl["name"]}");
    uint32 &gc_flag = *udata;
 
    if(gc_flag == 1)
    \{
        udata += 1;
        #{ns_cls_name} *obj = *(#{ns_cls_name}**)udata;
        delete obj;
    \}

    return 0;
\}
}
    end
  end
  gen_str += "
static void register_lua(lua_State *lua_state)
{"
  if $reg_func_tbl.has_key? "_"
    func_list = $reg_func_tbl["_"]
    gen_str += "
    /* register global namespace */"
    func_list.each do |export_name, func_name|
      gen_str += %Q{
    lua_pushcfunction(lua_state, #{func_name});
    lua_setglobal(lua_state, "#{export_name}");}
    end
    gen_str += "
"
  end
  gen_str += "
    /* register non-global namespace */"
  $reg_info.each_key do |full_name|
    gen_str += %Q{
    build_global_table(lua_state, "#{full_name}");} if full_name != "_" #except global namespace
  end
  gen_str += "
"
  $reg_func_tbl.each do |full_name, func_list|
    next if full_name == "_"
    reg_func_arr_name = full_name.gsub(/\./, "_")
    gen_str += %Q{
    \{
        luaL_Reg #{reg_func_arr_name}[] = 
        \{}
    func_list.each do |export_name, func_name|
      gen_str += %Q{
            \{"#{export_name}", #{func_name}\},}      
    end
    gen_str += %Q{
            \{NULL, NULL\}
        \};

        lua_settop(lua_state, 0);
        luaL_newmetatable(lua_state, "#{full_name}");
        luaL_setfuncs(lua_state, #{reg_func_arr_name}, 0);
        lua_setfield(lua_state, -1, "__index");
        get_global_table(lua_state, "#{full_name}");
        luaL_setfuncs(lua_state, #{reg_func_arr_name}, 0);
    \}
}
  end
  gen_str += "}
"
  File.open("./lua2cpp.cpp", "w").write(gen_str)
end

#parse and generate
parse_file
generate_file
