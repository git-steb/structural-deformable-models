# -----------------------------------------------------------------------
# defaults for the make process (by Martin Spindler Oct. 2002)
# -----------------------------------------------------------------------
#
# This file is supposed to hold the "default" Default settings for
# all systems.
#
# So instead of changing this file, it's a much better idea to put
# your configuration in your own project's config.make
#
# You might also want to take a look at [LINUX,WINDOWS,IRIX].config.make
#
# -----------------------------------------------------------------------

VERSION   ?= 0.1
#PLATFORM ?= i86 # this is not used at the moment

ifeq ($(NVDIA_EXT), YES)
   CPPFLAGS += -DNVIDIA_EXT
endif
