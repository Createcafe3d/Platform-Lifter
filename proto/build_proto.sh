#!/bin/bash

protoc -o messages.pb messages.proto
python ../nanopb/generator/nanopb_generator.py  messages.pb
protoc --python_out=../python/src/ messages.proto
mv messages.pb.* ../protobuf_arduino
