# --------------------------------------------------------------------
# makefile for creating libraries (by Martin Spindler Oct. 2002)
# --------------------------------------------------------------------
#
# input variables:
# ----------------
# TOOL_NAME   = list of tools, e.g. ...  = xxx yyy zzz
# xxx_SOURCES = source files,  e.g. ...  = main.c Class.cpp
# xxx_HEADERS = header files,  e.g. ...  = Class.h
# xxx_LIBS    = libraries,     e.g. ... += -lGL -lglut
# xxx_LIB_DIR = path to libs,  e.g. ... += -L/usr/lib
#
# INCLS    = path for headers, e.g. ... = -I/usr/include
# CPPFLAGS =
# CFLAGS   =
#
# default settings:
# -----------------
# xxx_LIBS    = $(LIBS)
# xxx_LIB_DIR = $(LIB_DIR)
#
# ------------------------------------------------------------------
# EXE     = if WINDOWS then EXE=.exe
# _OS_PATH_ = 
# LIBS
# LIB_DIR
# -----------------------------------------------------------------
#
#
#

# --------------------------------------------------------------------
# Remark: private varibales of this file have prefix "_tool"
# --------------------------------------------------------------------

_toolNames      = $(TOOL_NAME:%=%_SOURCES)
_toolProgNames  = $(TOOL_NAME:%=$(_OS_PATH_)/%$(EXE))
_toolTmp        = $(foreach i,$(_toolNames),$($(i):%.cpp=$(_OS_PATH_)/%.o))
_toolTmp2       = $(_toolTmp:%.cpp=$(_OS_PATH_)/%.o)
_toolObjects    = $(_toolTmp2:%.c=$(_OS_PATH_)/%.o)
_toolSources    = $(foreach i,$(_toolNames),$($(i)))
_toolDependFile = $(_OS_PATH_)/tool_depend

# --- main tool goals -----------------------------------------------

.PHONY: all clean depend distclean cleandepend cleangarbage

#dependTool
all: allTool

clean: cleanTool

depend: dependTool

distclean: distcleanTool

cleandepend: cleandependTool

cleangarbage: cleangarbageTool

# --- private tool rules --------------------------------------------

cleangarbageTool:
	$(RM) *~

ifdef TOOL_NAME
$(_toolProgNames): _toolName   = $(subst $(EXE),,$(notdir $@))
$(_toolProgNames): _toolDummy  = $($(_toolName)_SOURCES:%.cpp=$(_OS_PATH_)/%.o)
$(_toolProgNames): _toolDummy2 = $(_toolDummy:%.cxx=$(_OS_PATH_)/%.o)
$(_toolProgNames): _toolObj    = $(_toolDummy2:%.c=$(_OS_PATH_)/%.o)
$(_toolProgNames): _toolLibs   = $(LIBS) $($(_toolName)_LIBS)
$(_toolProgNames): _toolLibDir = $(LIB_DIR) $($(_toolName)_LIB_DIR)
$(_toolProgNames): $(_toolObjects)
# forceIt
	$(CC) -o $@ $(_toolObj) $(_toolLibDir) $(_toolLibs)
ifeq ($(debug),no)
	$(STRIP) $@
endif

#forceIt:

allTool: $(_OS_PATH_) $(_toolObjects) $(_toolProgNames)
	@touch $(_toolDependFile).sed
	@touch $(_toolDependFile)

cleanTool:
	$(RM) $(_toolObjects)
	$(RM) $(_toolProgNames)
	$(RM) $(_toolDependFile).sed
	$(RM) $(_toolDependFile)
	$(RM) -r $(_OS_PATH_)

dependTool: $(_toolDependFile)

distcleanTool: cleanTool
	$(RM) -r obj

cleandependTool:
	$(RM) $(_toolDependFile) $(_toolDependFile).sed

ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),distclean)
    ifneq ($(MAKECMDGOALS),cleandepend)
      ifneq ($(MAKECMDGOALS),cleangarbage)
        -include $(_toolDependFile)
      endif
    endif
  endif
endif

else
$(error TOOL_NAME is not set!)
endif

# --- tool private depend rules ---------------------------------------------

_toolEscTmp     :=$(subst /,\/,$(_OS_PATH_)/)
_toolEscObjPath :=$(subst  ,,$(strip $(subst .,\.,$(_toolEscTmp))))
_toolSedCmd     :="s/\([a-zA-Z0-9]*\)\.o:/$(_toolEscObjPath)\1\.o:/"

$(_toolDependFile): $(_toolDependFile).sed
	sed -e $(_toolSedCmd) $(_toolDependFile).sed > $(_toolDependFile)

$(_toolDependFile).sed: $(_OS_PATH_) $(_toolSources)
	$(CC) -MM -MG $(CPPFLAGS) $(INCLS) $(_toolSources) > $(_toolDependFile).sed

# -------------------------------------------------------------------

$(_OS_PATH_)/%.o : %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLS) -o $@ $<

$(_OS_PATH_)/%.o : %.cxx
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLS) -o $@ $<

$(_OS_PATH_)/%.o : %.c $(OS)
	$(CC) -c $(FLAGS) $(CPPFLAGS) $(INCLS) -o $@ $<

