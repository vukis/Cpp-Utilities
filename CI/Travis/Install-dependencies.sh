  # install gcc-7
  - sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test # gcc
  - sudo apt-get update -qq
  - sudo apt-get install -qq g++-7
  - sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 90
  - if [ "$CXX" = "g++" ]; then export CXX="g++-7"; fi
  
  #install boost 1.64.0
  - sudo wget -O boost_1_64_0.tar.gz http://sourceforge.net/projects/boost/files/boost/1.64.0/boost_1_64_0.tar.gz/download
  - sudo tar xzf boost_1_64_0.tar.gz
  - cd boost_1_64_0/
  - sudo ./bootstrap.sh --with-libraries=filesystem,system
  - sudo ./b2 toolset=gcc link=shared threading=multi variant=$BUILD_CONFIGURATION
  - sudo ./b2 install
  - cd ..
