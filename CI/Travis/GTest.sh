#!/bin/sh
git clone https://github.com/srouquette/googletest.git
cd googletest
chmod +x waf
./waf configure
./waf install_release
./waf install_debug

export CWD=`pwd`
export CPLUS_INCLUDE_PATH="$CWD/googletest/include:$CWD/googlemock/include:$CPLUS_INCLUDE_PATH"
export LIBRARY_PATH="$CWD/lib:$LIBRARY_PATH"

cd ..