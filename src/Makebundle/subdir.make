#---------------------------------------------------------------------
# makefile for calling sub directories (by Martin Spindler Oct. 2002)
#---------------------------------------------------------------------
#
# input variables:
#   SUBDIRS => name of subdirectories
#

# --- public rules ---------------------------------------------------

.PHONY: clean all depend distclean cleandepend cleangarbage $(SUBDIRS)

all: $(SUBDIRS)

clean: $(SUBDIRS)

depend: $(SUBDIRS)

distclean: $(SUBDIRS)

cleandepend: $(SUBDIRS)

cleangarbage: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

# $(MAKEFLAGS)