# -----------------------------------------------------------------------
# IRIX defaults for the make process (by Martin Spindler Oct. 2002)
# -----------------------------------------------------------------------

CPPFLAGS  += -DIRIX
NVIDIA_EXT = YES
CC         = g++

CPPFLAGS  += -DIRIX_GCC_3_0_4

INCLS     += -I/gruvi/usr/irix/include 
GLLIBS     = -lGL -lGLU -lglut
#WXDIR     = /gruvi/usr/linux/lib  not installed yet


ifeq ($(NVDIA_EXT), YES)
   CPPFLAGS += -DNVIDIA_EXT
endif

_INCL_ROOT = /usr/freeware/lib/gcc-lib/mips-sgi-irix6.5/3.0.4/include/g++

CFLAGSSSS += -W -Wall -Wpointer-arith -ffor-scope -Woverloaded-virtual \
          -Wmissing-prototypes \
         ${foo:+'-Wold-style-cast'} -Wwrite-strings \
         ${foo:+'-Winline -Wredundant-decls'} -fno-rtti -fno-exceptions \
         -finline-functions  -fforce-mem ${foo:+'-funsigned-char -fshort-enums'} \
-fforce-addr -felide-constructors ${foo:+'-fno-implicit-templates'} \
-fomit-frame-pointer ${foo:+'-fvtable-thunks'} ${foo:+'-Weffc++'} -freg-struct-return


#INCLS += -I/usr/include
#INCLS += -I/usr/include/CC

+INCLS += -I/home/dk12/graphics/current/include
INCLS += -I/usr/freeware/src/gcc-3.0.4/libstdc++-v3/libio
INCLS += -I/usr/local/lib/gcc-lib/mips-sgi-irix6.5/3.0.4/include

#INCLS += -I$(_INCL_ROOT)/mips-sgi-irix6.5/bits
#INCLS += -I$(_INCL_ROOT)/mips-sgi-irix6.5
#INCLS += -I$(_INCL_ROOT)/backward
#INCLS += -I$(_INCL_ROOT)

