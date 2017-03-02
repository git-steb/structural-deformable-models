---
---
Classification of ant species in macro-photographic images
==========================================================

Implementation of the image analysis system described in
Steven Bergner's thesis work on ["Deformable structural models"][bergner2003ants],
also published at IEEE ICIP 2004.

Travis-CI status: ![build status](https://travis-ci.org/git-steb/structural-deformable-models.svg?branch=master)

Algorithm
---------

The system involves feature detection layers with convolutions and non-linear functions of the underlying color image data input.

A combination of stochastic search and local gradient descent with damping
is used to optimize a graphical model of shape components that finds connections among
maxima in the feature detection sensor layer and compares them to dynamically adjustable models of different ant species.

An overview of the main classes implemented in the ``src`` folder is given in
Chapter 5 of the thesis ["Structural Deformable Models for Robust Object Recognition"
on page 61 (p. 81 in the PDF)](https://stevenbergner.github.io/assets/pub/thesis_bergner03.pdf "diploma thesis PDF file").

Also have a look at the [auto-generated documentation](https://git-steb.github.io/structural-deformable-models/).

![alt text][sdm-framework]


INSTALLATION 
------------

After adjusting build paths and compiler options in src/Makebundle/pre.make and lib.make run

    ./download-externals.sh
    cd src
    make -j4

to obtain a build of the deform binary in a architecture specific subfolder, e.g. ``src/obj/LINUX_debug/``.

Libraries and external source code that is required to build and run the application:

* FOX toolkit (build with ``./configure  --with-opengl``)
* FFTW3
* Eigen3
* currently deactivated EPS export: camgraph
* System installation of libglut and libGL is required. Also, libfreetype6-dev, etc.

Some of the package installations to compile using ``cd src; make -j4`` on Ubuntu 16.04:

    sudo apt install libfox-1.6-dev
    sudo apt install libpng-dev
    sudo apt install libfreetype6-dev
    sudo apt install libxft-dev
    ...

INSTALLATION - Data
-------------------

The algorithm works on image data, e.g., as collected by [MCZ at Harvard Entomology](insects.oeb.harvard.edu/mcz/ "database website").
Download of macro-photographic image data for personal use or research, or whichever type of usage permitted by the database website, can be done using a script (TODO) and wget.

LICENSE for the contents of this repository (MIT)
-------------------------------------------------

Copyright 2003, 2017 Steven Bergner

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[bergner2003ants]: https://stevenbergner.github.io/research/2003/12/18/deformable-models.html "diploma thesis research page"
[sdm-framework]: https://stevenbergner.github.io/assets/img/sdm-framework.jpg "Structural Deformable Model Framework"
