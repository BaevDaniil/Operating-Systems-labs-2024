#!/bin/bash
mkdir build
cd build
cmake -S ../ -B ./
make

mv host* ../
mv client* ../

cd ../
rm -r build

sudo chmod a+x host*
sudo chmod a+x client*
