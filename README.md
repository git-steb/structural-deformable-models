Classification of ant species in macro-photographic images
==========================================================

Implementation of the image analysis system described in
Steven Bergner's thesis work on "Deformable structural models",
also published at IEEE ICIP 2004.


TODO
----

* make code compile with even more modern c++
* write manual installation instructions
* help with installation of missing components:
  - src/fftw - replace with system installed version (e.g. using ubuntu/apt)
  - src/LinAlg - download from http://okmij.org/ftp/NumMath.html#cpp.linalg
  - src/camgraph - provides OpenGL -> EPS output for publications
  - data/mcz/*.jpg - make a download script, e.g. using pyquery
* make automatic installation work, integrate on Travis CI

LICENSE (MIT)
-------------

Copyright 2003, 2017 Steven Bergner

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
