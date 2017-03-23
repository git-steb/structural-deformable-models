#!/bin/bash

# Initialize external folder and stage dependencies
# Eigen, FFTW, fox-toolkit, FFTW, OpenGL, GLUT
# works on Linux and OSX

function confirm {
    if [ ! ${BASH_ARGV[0]} = "-q" ]; then
	read -p "Press enter to continue or Ctrl-C to abort: "
    fi
}
unamestr=$(uname)

# download dependencies and install
pdir=$(dirname $0)
mkdir external
pushd external

eigenfile=3.3.3.tar.gz
eigenfolder=eigen-eigen-67e894c6cd8f
eigenurl=https://bitbucket.org/eigen/eigen/get/$eigenfile
echo "Downloading and installing Eigen to " $pdir/external/eigen
if [[ "$unamestr" == 'Darwin' ]]; then
    # workaround, because there is no SSL capable curl on OSX, by defaul
    echo "Using Safari to download Eigen3, then unpacking and staging to external."
    echo "Note: You should switch back to this shell after the download completed."
    confirm
    open -a Safari $eigenurl
    echo "Waiting for ~/Downloads/$eigenfolder.tar to exist"
    while [ ! -f ~/Downloads/$eigenfolder.tar ]
    do
	sleep 1
    done
    tar -xf ~/Downloads/$eigenfolder.tar
else
    wget $eigenurl -O eigen-3.3.3.tgz
    tar xzf eigen-3.3.3.tgz
fi
mv $eigenfolder eigen

if [[ "$unamestr" == 'Darwin' ]]; then
    echo
    echo "Installing fox-toolkit, FFTW, and freeglut using homebrew"
    confirm
    brew install fox fftw freeglut

    openglinc=/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers
    if [ -d $openglinc ]; then
	mkdir include
	ln -s $openglinc include/GL
    else
	echo "Could not find OpenGL headers at location"
	echo $openglinc
    fi
fi

popd

echo
echo "External dependencies successfully installed."
echo "You can build the app now, e.g., using: cd src; make -j4 release"
echo
