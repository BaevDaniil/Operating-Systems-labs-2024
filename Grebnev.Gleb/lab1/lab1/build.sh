#!/bin/bash

rm -rf *.o daemon

g++ -Wall -Werror -o daemon daemon.cpp

if [ $? -eq 0 ]; then
    echo "—борка успешна."
else
    echo "ќшибка сборки."
fi