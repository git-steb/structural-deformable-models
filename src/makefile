# The makefile for the whole volume project.

#------------------------------------------------------------------------
#Include the general build configuration

MAKEBUNDLE = ./Makebundle
srcdir = .
SRCDIR = $(srcdir)
include $(MAKEBUNDLE)/pre.make

SUBDIRS =
#../external/camgraph
include $(MAKEBUNDLE)/subdir.make

ifneq ($(foo),yes)
ifneq ($(browser),yes)
TOOL_NAME = 	deform
else
TOOL_NAME = 	browser
endif
else
TOOL_NAME =	foo
endif

deform_SOURCES = deform.cpp						\
		 Node.cpp Model.cpp vuThread.cpp Data.cpp Sensor.cpp	\
		 Brain.cpp SensorSet.cpp SensorColl.cpp SpeciesDB.cpp	\
		 ExpMap.cpp Searcher.cpp MStruct.cpp Fourier.cpp	\
		 sensordlg.cpp utils.cpp StructTable.cpp

foo_SOURCES = foo.cpp							\
		 Node.cpp Model.cpp vuThread.cpp Data.cpp Sensor.cpp	\
		 Brain.cpp SensorSet.cpp SensorColl.cpp SpeciesDB.cpp	\
		 ExpMap.cpp Searcher.cpp MStruct.cpp Fourier.cpp	\
		 utils.cpp StructTable.cpp

browser_SOURCES = browser.cpp						\
		 Node.cpp Model.cpp vuThread.cpp Data.cpp Sensor.cpp	\
		 Brain.cpp SensorSet.cpp SensorColl.cpp SpeciesDB.cpp	\
		 ExpMap.cpp Searcher.cpp MStruct.cpp Fourier.cpp	\
		 utils.cpp StructTable.cpp

CPPFLAGS +=  -Wall -Wno-unused
ifeq ($(debug),no)
CPPFLAGS += -O3 -funroll-loops -DNDEBUG
#-DNOTRACE
#CPPFLAGS += -mmmx -m3dnow -msse
LDFLAGS += -O3
endif

ifeq ($(PROF),1)
CPPFLAGS += -pg
LDFLAGS += -pg
endif

include $(MAKEBUNDLE)/tool.make

release:
	+make debug=no
