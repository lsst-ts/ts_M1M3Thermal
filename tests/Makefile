include ../Makefile.inc

all: compile

.PHONY: FORCE compile run junit clean

TEST_SRCS := $(shell ls test_*.cpp 2>/dev/null)
BINARIES := $(patsubst %.cpp,%,$(TEST_SRCS))
DEPS := $(patsubst %.cpp,%.cpp.d,$(TEST_SRCS))
JUNIT_FILES := $(shell ls *.xml 2>/dev/null)

ifneq ($(MAKECMDGOALS),clean)
    -include $(DEPS)
endif

LIBS += $(shell pkg-config catch2-with-main --libs)

M1M3_CPPFLAGS := -I"../${CRIOCPP}/include" \
	$(shell pkg-config catch2-with-main --cflags) \
	-I"../src/NiFpga" \
	-I"../src/LSST/M1M3/TS" \

compile: $(BINARIES)

run: compile
	@$(foreach b,$(BINARIES),echo '[RUN] ${b}'; ./${b};)

junit: compile
	@$(foreach b,$(BINARIES),echo '[JUT] ${b}'; ./${b} -r junit -o ${b}.xml;)

clean:
	@$(foreach df,$(BINARIES) $(patsubst %,%.cpp.o,$(BINARIES)) $(DEPS) $(JUNIT_FILES),echo '[RM ] ${df}'; $(RM) ${df};)

../src/libM1M3SS.a: FORCE
	@$(MAKE) -C ../src libM1M3TS.a SIMULATOR=1

%.cpp.o: %.cpp.d
	@echo '[CPP] $(patsubst %.d,%,$<)'
	${co}$(CPP) $(BOOST_CPPFLAGS) $(SAL_CPPFLAGS) $(M1M3_CPPFLAGS) -c -fmessage-length=0 -o $@ $(patsubst %.d,%,$<)

%.cpp.d: %.cpp
	@echo '[DPP] $<'
	${co}$(CPP) $(BOOST_CPPFLAGS) $(SAL_CPPFLAGS) $(M1M3_CPPFLAGS) -M $< -MF $@ -MT '$(patsubst %.cpp,%.o,$<) $@'

${BINARIES}: %: %.cpp.o ../src/libM1M3TS.a
	@echo '[TPP] $<'
	${co}$(CPP) -o $@ $(LIBS_FLAGS) $(BOOST_CPPFLAGS) $(SAL_CPPFLAGS) $(SAL_LIBS) $(M1M3_CPPFLAGS) $^ $(LIBS) ../$(CRIOCPP)/lib/libcRIOcpp.a ${SAL_WORK_DIR}/lib/libSAL_MTM1M3TS.a
