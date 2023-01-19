include ../Makefile.inc

all: libM1M3TS.a

.PHONY: clean force

VERSION := $(shell git describe --tags --dirty 2>/dev/null || echo "unknown:non-git")
GIT_HASH := $(shell git rev-parse HEAD 2>/dev/null || echo "unknown-non-git")

version.c: force
	${co}grep -qsF "VERSION=\"$(VERSION)\"" version.c \
	|| echo "[VER] version $(VERSION) git hash $(GIT_HASH)" && \
	printf >version.c 'const char *VERSION="%s";\nconst char *GIT_HASH="%s";' $(VERSION) $(GIT_HASH)

CFLAGS += ${SAL_CPPFLAGS} \
	-I../../ts_cRIOcpp/include \
	-ILSST/M1M3/TS \
	-INiFpga \

C_SRCS = $(shell find LSST NiFpga -name '*.c')
LIB_CPP_SRCS = $(shell find LSST NiFpga -name '*.cpp') 
ALL_CPP_SRCS = $(LIB_CPP_SRCS) $(shell ls *.cpp)

LIB_OBJS = $(patsubst %.c,%.c.o,$(C_SRCS)) $(patsubst %.cpp,%.cpp.o,$(LIB_CPP_SRCS)) version.c.o
ALL_OBJS = $(patsubst %.c,%.c.o,$(C_SRCS)) $(patsubst %.cpp,%.cpp.o,$(ALL_CPP_SRCS))

C_DEPS = $(patsubst %.c,%.c.d,$(C_SRCS))
CPP_DEPS = $(patsubst %.cpp,%.cpp.d,$(ALL_CPP_SRCS))

.PRECIOUS: *.d

ifneq ($(MAKECMDGOALS),clean)
  -include ${C_DEPS} ${CPP_DEPS}
endif

libM1M3TS.a: $(LIB_OBJS)
	@echo '[AR ] $@'
	${co}$(AR) rs $@ $^

clean:
	@$(foreach file,$(ALL_OBJS) $(C_DEPS) $(CPP_DEPS) libM1M3TS.a version.c $(shell find -name '*.d'), echo '[RM ] ${file}'; $(RM) -r $(file);)

clang-format:
	@$(foreach file,$(shell find LSST -name '*.cpp' -o -name '*.h'), echo '[FRM] ${file}'; clang-format -i $(file);)

%.cpp.o: %.cpp.d
	@echo '[CPP] $(patsubst %.d,%,$<)'
	${co}$(CPP) $(CRIO_CFLAGS) $(CFLAGS) -c -o $@ $(patsubst %.d,%,$<)

%.c.o: %.c.d
	@echo '[C  ] $(patsubst %.d,%d,$<)'
	${co}$(C) -c -fmessage-length=0 -o $@ $(patsubst %.d,%,$<)

%.cpp.d: %.cpp
	@echo '[DEP] $<'
	${co}$(CPP) $(CRIO_CFLAGS) $(CFLAGS) -M $< -MF $@ -MT '$(patsubst %.cpp,%.o,$<) $@'

%.c.d: %.c
	@echo '[DEP] $<'
	${co}$(C) -M $< -MF $@ -MT '$(patsubst %.c,%.o,$<) $@'
