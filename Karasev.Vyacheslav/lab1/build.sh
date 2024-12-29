#!/bin/bash

rm -f daemon

g++ -Wall -Werror -o daemon main.cpp daemon.cpp
if [ $? -eq 0 ]; then
    echo "Build successful"
else
    echo "Build failed"
fi

rm -f *.o