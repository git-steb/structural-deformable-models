#---------------------------------------------------------------------
# makefile for creating libraries (by Martin Spindler Oct. 2002)
#---------------------------------------------------------------------
#
# input variables:
#
#   HEADERS    => *.h files
#   SOURCES    => *.cpp *.c *.cxx files
#   LIB_NAME   => lib name
#   LIB_PREFIX => prefix for *.o files ("*.o" -> $(LIB_PREFIX)_*.o)
# --------------------------------------------------------------------
#
# OS        =
# _LIB_PATH_  = installation path of lib
#
#

# --------------------------------------------------------------------
# Remark: private varibales of this file have prefix "_lib"
# --------------------------------------------------------------------

_libName  = lib$(LIB_NAME).a

ifdef LIB_PREFIX
  _libObjPath = $(_OS_PATH_)/$(LIB_PREFIX)_
else
  _libObjPath = $(_OS_PATH_)/
endif

_libTmp     = $(SOURCES:%.cpp=$(_libObjPath)%.o)
_libTmp2    = $(_libTmp:%.cxx=$(_libObjPath)%.o)
_libTmp3    = $(_libTmp2:%.cc=$(_libObjPath)%.o)
_libObjects = $(_libTmp3:%.c=$(_libObjPath)%.o)

# --- main lib goals ------------------------------------------------

.PHONY: clean all depend distclean cleandepend cleangarbage

all: dependSOURCES allSOURCES

clean: cleanSOURCES

depend: dependSOURCES

distclean: distcleanSOURCES

cleandepend: cleandependSOURCES

cleangarbage: cleangarbageSOURCES

# --- SOURCES RULES ---------------------------------------------

cleangarbageSOURCES:
	$(RM) *~

ifdef SOURCES
allSOURCES: $(_OS_PATH_)                             \
            $(_LIB_PATH_)                            \
            $(_LIB_PATH_)/$(_libName)($(_libObjects))
	@touch $(_OS_PATH_)/depend.sed
	@touch $(_OS_PATH_)/depend

cleanSOURCES: 
	$(RM) $(_libObjects)
	$(RM) $(_OS_PATH_)/depend 
	$(RM) $(_OS_PATH_)/depend.sed
	$(RM) -r $(_OS_PATH_)
#	$(RM) $(_LIB_PATH_)/$(_libName)

dependSOURCES: $(_OS_PATH_)/depend

distcleanSOURCES: cleanSOURCES
	$(RM) -r obj

cleandependSOURCES:
	$(RM) $(_OS_PATH_)/depend $(_OS_PATH_)/depend.sed

ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),distclean)
    ifneq ($(MAKECMDGOALS),cleandepend)
      ifneq ($(MAKECMDGOALS),cleangarbage)
        -include $(_OS_PATH_)/depend
      endif
    endif
  endif
endif

else
allSOURCES:

cleanSOURCES:

dependSOURCES:

distcleanSOURCES:

cleandependSOURCES:

endif


# --- lib private depend rules ---------------------------------------------

_libEscTmp     :=$(subst /,\/,$(_libObjPath))
_libEscObjPath :=$(subst  ,,$(strip $(subst .,\.,$(_libEscTmp))))
_libSedCmd     :="s/\([a-zA-Z0-9_]*\)\.o:/$(_libEscObjPath)\1\.o:/"

$(_OS_PATH_)/depend: $(_OS_PATH_)/depend.sed
	sed -e $(_libSedCmd) $(_OS_PATH_)/depend.sed > $(_OS_PATH_)/depend

$(_OS_PATH_)/depend.sed: $(_OS_PATH_) $(SOURCES)
	$(CC) -MM -MG $(CPPFLAGS) $(SOURCES) $(INCLS) > $(_OS_PATH_)/depend.sed

# ---------------------------------------------------------------------

($(_OS_PATH_)/%.o): $(_libObjPath)%.o
	$(AR) rcvs $@ $(_OS_PATH_)/$*.o

#-----------------------------------------------------------------------

$(_libObjPath)%.o : %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLS) -o $@ $<

$(_libObjPath)%.o : %.cxx
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLS) -o $@ $<

$(_libObjPath)%.o : %.c 
	$(CC) -c $(FLAGS) $(CPPFLAGS) $(INCLS) -o $@ $<

$(_libObjPath)%.o : %.cc 
	$(CC) -c $(FLAGS) $(CPPFLAGS) $(INCLS) -o $@ $<

