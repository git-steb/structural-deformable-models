#!/bin/bash

# download dependencies and install
pdir=$(dirname $0)
mkdir external

echo "Downloading and installing Eigen to " $pdir/external/eigen
pushd external
wget http://bitbucket.org/eigen/eigen/get/3.3.3.tar.gz -O eigen-3.3.3.tgz
tar xzf eigen-3.3.3.tgz 
mv eigen-eigen-67e894c6cd8f eigen
popd
