#!/usr/bin/env bash

protoc -I=./proto_files --cpp_out=./generated/ ./proto_files/*/*/*/*.proto
protoc -I=./proto_files --cpp_out=./generated/ ./proto_files/*/*/*.proto
protoc -I=./proto_files --lua_out=./generated/ ./proto_files/*/*/*/*.proto
protoc -I=./proto_files --lua_out=./generated/ ./proto_files/*/*/*.proto
