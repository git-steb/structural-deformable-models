#!/usr/bin/env bash

swig -c++ -lua -I./../external/eigen -I/usr/include/fox-1.6 deform.i

g++ -c deform_wrap.cxx -DLINUX -Wall  -std=c++0x -DHAVE_GL_H -DHAVE_GLU_H -g -Wall -Wno-unused -I. -I./../external/eigen -I/usr/include/fox-1.6 -std=c++0x -llua5.3 -ldl -Wall -I/usr/include/lua5.1 -fPIC

make -j4 release

g++ -shared -o deform.so deform_wrap.o obj/LINUX/deform.o obj/LINUX/Node.o obj/LINUX/Model.o obj/LINUX/vuThread.o obj/LINUX/Data.o obj/LINUX/Sensor.o obj/LINUX/Brain.o obj/LINUX/SensorSet.o obj/LINUX/SensorColl.o obj/LINUX/SpeciesDB.o obj/LINUX/ExpMap.o obj/LINUX/Searcher.o obj/LINUX/MStruct.o obj/LINUX/Fourier.o obj/LINUX/sensordlg.o obj/LINUX/utils.o obj/LINUX/StructTable.o -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu -L./lib/obj/LINUX  -lglut -lGLU -lGL -lFOX-1.6 -lpthread -lfftw3 -fPIC
