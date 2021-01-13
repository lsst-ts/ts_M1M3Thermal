include ../Makefile.inc

all: m1m3tscli

CFLAGS += -ILSST/M1M3/TS -INiFpga

C_SRCS = $(shell find LSST NiFpga -name '*.c')
CPP_SRCS = $(shell find LSST NiFpga -name '*.cpp')

OBJS = $(patsubst %.c,%.c.o,$(C_SRCS)) $(patsubst %.cpp,%.cpp.o,$(CPP_SRCS))

C_DEPS = $(patsubst %.c,%.c.d,$(C_SRCS))
CPP_DEPS = $(patsubst %.cpp,%.cpp.d,$(CPP_SRCS))

.PRECIOUS: *.d

ifneq ($(MAKECMDGOALS),clean)
  -include ${C_DEPS} ${CPP_DEPS}
endif

m1m3tscli: m1m3tscli.cpp.o libM1M3TS.a
	@echo '[LD ] $@'
	${co}$(CPP) -o $@ $^ ../../ts_cRIOcpp/src/libcRIOcpp.a -lreadline

libM1M3TS.a: $(OBJS)
	@echo '[AR ] $@'
	${co}$(AR) r $@ $^
	@echo '[RLB] $@'
	${co}ranlib $@

clean:
	@$(foreach file,$(OBJS) $(C_DEPS) $(CPP_DEPS) libM1M3SS.a $(shell find -name '*.d'), echo '[RM ] ${file}'; $(RM) -r $(file);)

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
