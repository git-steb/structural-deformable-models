# -----------------------------------------------------------------------
# LINUX defaults for the make process (by Martin Spindler Oct. 2002)
# -----------------------------------------------------------------------

CPPFLAGS  += -DLINUX -Wall
CC         = g++
STRIP      = strip

LIB_DIR    = -L/usr/local/lib
LIBS       = -lglut -lGLU -lGL -lFOX -lpthread -lla -lcamgraph -lfftw3
INCLS      = -I/usr/local/include/ -I/usr/local/include/fox -I.

#GLLIBS     = -lGL -lGLU -lglut
