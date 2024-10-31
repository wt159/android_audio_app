#!/bin/bash

projectdir=$(pwd)
echo $projectdir

if [ ! -d build ]; then
	mkdir -p build
fi

cd build
cmake ..
make

cd ..

if [ ! -d bin ]; then
	mkdir -p bin
fi

mv build/bin/si479x_radio ./bin
mv build/bin/flash_tool ./bin
