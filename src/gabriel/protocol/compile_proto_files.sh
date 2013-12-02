#!/usr/bin/env bash

protoc -I=./proto_files --cpp_out=./generated/ ./proto_files/supercenter/*.proto
protoc -I=./proto_files --cpp_out=./generated/ ./proto_files/center/*.proto
protoc -I=./proto_files --cpp_out=./generated/ ./proto_files/login/*.proto
protoc -I=./proto_files --cpp_out=./generated/ ./proto_files/record/*.proto
protoc -I=./proto_files --cpp_out=./generated/ ./proto_files/game/*.proto
protoc -I=./proto_files --cpp_out=./generated/ ./proto_files/gateway/*.proto
