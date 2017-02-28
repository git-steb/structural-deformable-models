# -----------------------------------------------------------------------
# makefile for configuring the make process (by Martin Spindler Oct. 2002)
# -----------------------------------------------------------------------
#
# MAKEBUNDLE  e.g. ... = ~/Projects/Makefiles
# OS_TYPE     e.g. ... = Linux
# debug       e.g. ... = no (default=YES)
#
# srcdir
# SRCDIR
#

include $(MAKEBUNDLE)/detect-os-type.make

# ---------------------------------------------------------------------

OS = undefined

ifeq ($(OS_TYPE), Linux) 
  OS = LINUX
endif
ifeq ($(OS_TYPE), IRIX64)
  OS = IRIX
endif
ifeq ($(OS_TYPE), WINDOWS)
  OS = WINDOWS
endif

ifeq ($(OS),undefined)
$(error OS is unknown! Please check your environment varibale OS_TYPE [valid types are: Linux, IRIX64, WINDOWS])
endif


# -----------------------------------------------------------------------
# include config files

_preConfigFile = $(OS).config.make

include $(MAKEBUNDLE)/$(_preConfigFile)
include $(MAKEBUNDLE)/config.make
ifdef srcdir
  -include $(srcdir)/$(_preConfigFile)
  -include $(srcdir)/config.make
endif
-include $(_preConfigFile)
-include config.make

# -----------------------------------------------------------------------
# ensure some variables

ifndef SRCDIR
  $(error SRCDIR is not set)
endif

ifndef INCLS
  $(error INCLS is not set)
endif

ifndef CC
  $(error CC is not set)
endif

ifndef CPPFLAGS
  $(error CPPFLAGS is not set)
endif

ifndef STRIP
  STRIP = echo
endif

ifneq ($(debug),no)
  CPPFLAGS += -g
endif

# --- _OS_PATH_ settings --------------------------------------------------

ifdef PLATFORM
  ifneq ($(debug),no)
    _OS_SPATH_ = $(PLATFORM)_$(OS)_debug
  else
    _OS_SPATH_ = $(PLATFORM)_$(OS)
  endif
else
  ifneq ($(debug),no)
    _OS_SPATH_ = $(OS)_debug
  else
    _OS_SPATH_ = $(OS)
  endif
endif

_OS_PATH_ = obj/$(_OS_SPATH_)

all: # the first found target is the "default" target -> in our case it's "all"

obj:
	-mkdir obj

$(_OS_PATH_): obj
	-mkdir -p $(_OS_PATH_)

# --- library settings --------------------------------------------------
# _LIB_PATH_ is private in the Makebundle:

_LIB_PATH_ := $(SRCDIR)/lib/$(_OS_PATH_)

$(SRCDIR):
	@if (test ! -e $(SRCDIR)); then \
	  mkdir $(SRCDIR);             \
	fi

$(SRCDIR)/lib: $(SRCDIR)
	@if (test ! -e $(SRCDIR)/lib); then \
	  mkdir $(SRCDIR)/lib;             \
	fi

$(SRCDIR)/lib/obj: $(SRCDIR)/lib
	@if (test ! -e $(SRCDIR)/lib/obj); then \
	  mkdir $(SRCDIR)/lib/obj;             \
	fi

$(_LIB_PATH_): $(SRCDIR)/lib/obj
	@if (test ! -e $(_LIB_PATH_)); then \
	  mkdir $(_LIB_PATH_);             \
	fi

# add _LIB_PATH_ to standard library path:
LIB_DIR   += -L$(_LIB_PATH_)

#------------------------------------------------------------------------
#Rules for building different files

%.o : %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLS) -o $@ $<

%.o : %.cxx
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLS) -o $@ $<

%.o : %.c $(OS)
	$(CC) -c $(FLAGS) $(CPPFLAGS) $(INCLS) -o $@ $<

%.o : %.cc
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLS) -o $@ $<
