# -----------------------------------------------------------------------
# WINDOWS defaults for the make process (by Martin Spindler Oct. 2002)
# -----------------------------------------------------------------------

LDFLAGS += -Wall -W -Wmissing-prototypes -Woverloaded-virtual -Wformat -DHAVE_OPENGL -mwindows -e _mainCRTStartup
LIB_DIR += -L/usr/local/lib -L/d/common/glut -L./fftw/lib
#LIBS =  -lFOX -lopengl32 -lglu32 -lm -lm -lcomctl32 -lwsock32 -lwinspool -lmpr -lm
LIBS += -lFOX -lopengl32 -lglu32 -lglut -lcomctl32 -lwsock32 -lwinspool -lmpr -lm -lpng -ljpeg -ltiff -lz -lfftw3 -lm -lla -lcamgraph $(LDFLAGS)
#LIBS += -lFOX -lopengl32 -lglu32 -lglut -lcomctl32 -lwsock32 -lwinspool -lmpr -lm -lpng -ljpeg -ltiff -lz -lfftw3 -lm -lla $(LDFLAGS)
INCLS += -I/usr/local/include/fox -I./fftw
#-I/d/common/stl
CPPFLAGS += -DWIN32

#GLLIBS   = -lglu32 -lopengl32 -lglut
EXE       = .exe
CC	  = g++
STRIP	  = strip
