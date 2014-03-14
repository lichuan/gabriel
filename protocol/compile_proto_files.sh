#!/usr/bin/env bash

protoc -I=./proto_files --cpp_out=./generated/ ./proto_files/*/*/*.proto ./proto_files/*/*/*/*.proto
