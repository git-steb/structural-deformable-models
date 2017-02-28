# -----------------------------------------------------------------------
# The operating system we're building under. IRIX, LINUX or WINDOWS

ifeq ($(OS),Windows_NT)
    export OS_TYPE = WINDOWS
    #ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
    #    ...
    #else
    #    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
    #        ...
    #    endif
    #    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
    #        ...
    #    endif
    #endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
      export OS_TYPE = Linux
    endif
    ifeq ($(UNAME_S),Darwin)
      export OS_TYPE = Darwin
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif
