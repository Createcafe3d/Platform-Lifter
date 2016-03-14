#!/bin/bash

protoc -o messages.pb messages.proto
if [ $? != 0 ]; then
    echo "DEATH DOOM FAILURE"
    EXIT_CODE=101
    failed_exit
fi
python ../nanopb/generator/nanopb_generator.py  messages.pb
if [ $? != 0 ]; then
    echo "DEATH DOOM FAILURE"
    EXIT_CODE=102
    failed_exit
fi
protoc --python_out=../python/src/ messages.proto
if [ $? != 0 ]; then
    echo "DEATH DOOM FAILURE"
    EXIT_CODE=103
    failed_exit
fi
mv messages.pb.* ../lib/PeachDuino
if [ $? != 0 ]; then
    echo "DEATH DOOM FAILURE"
    EXIT_CODE=104
    failed_exit
fi
