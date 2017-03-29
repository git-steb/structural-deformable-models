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

![][sdm-framework]

Sensors can be convolutions or non-linear functions of source sensors in this bottom-up data flow architecture.
![][sensor-flow]

Shape part models of all species are related in stochastic scene graph.
Highly scoring paths indicate that the image data can be interpreted as a particular species.
![][ant-structure-graph]

A statistical model of relative shape position is attached to each edge in the stochastic scene graph.
![][stochastic-scene-graph-exp-map]

A grid in (x, y, rotation, scale) feature space is used to determine local winners
![][leaf-winners-in-grid-bins]

Color samples of ant body and background are acquired interactively
![][ant-colsample1]

Scatter plot of ant body color samples
![][antskincol-scatter]

Mahalanobis distance iso-surface of Gaussian model for background color samples
![][antskincol-bg]

Posterior probability iso-surface of combined Gaussian model for ant body color samples
![][antskincol-ant]


INSTALLATION 
------------

If you are lucky, you can [install dependencies](.travis.yml) and just build

Linux:

    sudo apt install freeglut3-dev mesa-common-dev libfox-1.6-dev libfftw3-dev libgl1-mesa-glx

OSX (using [homebrew](https://brew.sh/)):

    brew install fox fftw freeglut

Then:

    ./download-externals.sh
    cd src
    make -j4 release <OR> make -j4

This builds the ``deform`` binary in a architecture and build
variant specific subfolder, e.g. ``src/obj/LINUX``, ``src/obj/LINUX_debug``, or ``src/obj/DARWIN``.

If that does not work, you may have to adjust build paths and compiler options in
``src/Makebundle/<PLATFORM>.config.make`` or the remaining Makebundle infrastructure.

Libraries and external sources required to build and run the application:

* FOX toolkit (if building manually use at least ``./configure  --with-opengl``)
* FFTW3 - for fast Fourier transforms
* Eigen3 - for easy to use linear algebra
* System installation of OpenGL - to see the algorithm in action
* (optional) Camgraph  - for EPS vector graphics export (deactivated)

Have a look at [``.travis.yml``](.travis.yml) to see how dependencies are installed
automatically on the [Travis-CI build-bot](https://travis-ci.org/git-steb/structural-deformable-models) running [Ubuntu](https://www.ubuntu.com/download) 14.04 (precise).

Compilation under Windows 10 [using its new Bash](http://www.windowscentral.com/how-install-bash-shell-command-line-windows-10) should also be possible (TODO update instructions here).

INSTALLATION - Data
-------------------

The algorithm works on image data, e.g., as collected by [MCZ at Harvard Entomology](http://insects.oeb.harvard.edu/mcz/ "database website").
Download of macro-photographic image data for personal use or research, or whichever type of usage permitted by the database website, can be done using a script (TODO) and ``wget``.

LICENSE for the contents of this repository (MIT)
-------------------------------------------------

Copyright 2003, 2017 Steven Bergner

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[bergner2003ants]: https://stevenbergner.github.io/research/2003/12/18/deformable-models.html "Diploma thesis project page"
[sdm-framework]: https://stevenbergner.github.io/assets/img/sdm-framework.jpg "Structural Deformable Model Framework"
[ant-structure-graph]: https://stevenbergner.github.io/assets/img/ant-structure-graph.jpg "Shape part models of all species are related in stochastic scene graph. Highly scoring paths indicate that the image data can be interpreted as a particular species."
[sensor-flow]: https://stevenbergner.github.io/assets/img/sensor-flow.jpg "Sensors can be convolutions or non-linear functions of source sensors in this bottom-up data flow architecture."
[stochastic-scene-graph-exp-map]: https://stevenbergner.github.io/assets/img/stochastic-scene-graph-exp-map.jpg "A statistical model of relative shape position is attached to each edge in the stochastic scene graph."
[leaf-winners-in-grid-bins]: https://stevenbergner.github.io/assets/img/leaf-winners-in-grid-bins.jpg "A grid in (x, y, rotation, scale) feature space is used to determine local winners"
[ant-colsample1]: https://stevenbergner.github.io/assets/img/ant-colsample1.jpg "Color samples of ant body and background are acquired interactively"
[ant-colsample2]: https://stevenbergner.github.io/assets/img/ant-colsample2.jpg "Color samples of ant body and background are acquired interactively"
[antskincol-ant]: https://stevenbergner.github.io/assets/img/antskincol-ant.png "Mahalanobis distance iso-surface of Gaussian model for ant body color samples"
[antskincol-bg]: https://stevenbergner.github.io/assets/img/antskincol-bg.png "Mahalanobis distance iso-surface of Gaussian model for background color samples"
[antskincol-both]: https://stevenbergner.github.io/assets/img/antskincol-both.png "Combined and body color model"
[antskincol-scatter]: https://stevenbergner.github.io/assets/img/antskincol-scatter.png "Scatter plot of ant body color samples"
