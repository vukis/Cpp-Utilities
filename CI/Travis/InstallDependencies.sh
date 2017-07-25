#!/bin/sh
#install boost 1.64.0
sudo wget -O boost_1_64_0.tar.gz http://sourceforge.net/projects/boost/files/boost/1.64.0/boost_1_64_0.tar.gz/download
sudo tar xzf boost_1_64_0.tar.gz
cd boost_1_64_0/
sudo ./bootstrap.sh --with-libraries=filesystem,system
sudo ./b2 toolset=gcc link=shared threading=multi variant=$BUILD_CONFIGURATION
sudo ./b2 install
cd ..

# install codecov
sudo update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-5 90