#!/bin/bash
mkdir build
cd build
cmake -S ../ -B ./
make

rm -r host*autogen
rm -r client*autogen

mv host* ../
mv client* ../

cd ../
rm -r build

sudo chmod a+x host*
sudo chmod a+x client*
