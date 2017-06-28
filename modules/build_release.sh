#! /usr/bin/env bash

# where to install module libraries
# set this path in your vtapi.conf
INSTALL_PREFIX="/usr/local/lib"

BASEPATH="$( cd "$( dirname "${0}" )" && pwd )"
cd ${BASEPATH}
mkdir -p build/release
mkdir -p $INSTALL_PREFIX
cd build/release
cmake ../.. -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX
make
sudo make install