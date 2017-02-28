# -----------------------------------------------------------------------
# LINUX defaults for the make process (by Martin Spindler Oct. 2002)
# -----------------------------------------------------------------------

CPPFLAGS  += -DLINUX -Wall -std=c++11 -DHAVE_GL_H -DHAVE_GLU_H
CC         = g++
STRIP      = strip

LIB_DIR    = -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu
LIBS       = -lglut -lGLU -lGL -lFOX-1.6 -lpthread -lla -lfftw3
INCLS      = -I. -I$(SRCDIR)/../external/eigen -I/usr/local/include/fox-1.6

#GLLIBS     = -lGL -lGLU -lglut
