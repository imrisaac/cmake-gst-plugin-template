#!/bin/bash
#
# This script builds the gst-objecttrack plugin and generates a debian package from a fresh clone
#

rm -rf build
mkdir build

cd build
cmake ..
make -j
make debgen

echo "Build and Package generation complete"
echo " "
echo "Install command: "
echo "sudo cd build && make debinstall"