#!/bin/sh
git clone https://github.com/C-Chads/tinygl.git ./tinygl
cd ./tinygl/src
gcc -O3 -c *.c 
ar rcs libTinyGL.a *.o
cp libTinyGL.a ../../src/runtime/_tinygl.a
rm -rf ./tinygl