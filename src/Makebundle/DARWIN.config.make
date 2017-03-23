# -----------------------------------------------------------------------
# LINUX defaults for the make process (by Martin Spindler Oct. 2002)
# -----------------------------------------------------------------------

CPPFLAGS  += -DDARWIN -Wall -std=c++0x -DHAVE_GL_H -DHAVE_GLU_H -D__USE_ISOC99 \
             -Wno-tautological-undefined-compare
CC         = g++
STRIP      = strip

BREWPATH   = $(shell brew --prefix)
LIB_DIR    = -L$(BREWPATH)/lib
LIBS       = -framework OpenGL -lglut -lFOX-1.6 -lpthread -lfftw3
INCLS      = -I. -I$(BREWPATH)/include/eigen3 -I$(BREWPATH)/include \
             -I$(BREWPATH)/include/fox-1.6 -I$(SRCDIR)/../external/include
# -I/System/Library/Frameworks/OpenGL.framework/Headers

#GLLIBS     = -lGL -lGLU -lglut
