#!/usr/bin/env lua

package.cpath = "./protobuf/?.so"
package.path = "./generated/?.lua;./protobuf/?.lua;"
require("gabriel.protocol.server.db_pb")
local dd = gabriel.protocol.server.db_pb
print(dd)
local msg = dd.DB_Context()
msg.seq = 999999
msg.client_id =2323
print(msg)

