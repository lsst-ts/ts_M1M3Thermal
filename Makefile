include Makefile.inc

.PHONY: all clean deploy tests FORCE doc

# Add inputs and outputs from these tool invocations to the build variables 
#

# All Target
all: ts_M1M3Thermal $(m1m3cli)

src/libM1M3SS.a: FORCE
	$(MAKE) -C src libM1M3SS.a

# Tool invocations
ts_M1M3Thermal: src/ts_M1M3Thermal.cpp.o src/libM1M3TS.a
	@echo '[LD ] $@'
	${co}$(CPP) $(LIBS_FLAGS) -o $@ $^ $(LIBS)

# Other Targets
clean:
	@$(foreach file,ts_M1M3Thermal src/ts_M1M3Thermal.cpp.o doc, echo '[RM ] ${file}'; $(RM) -r $(file);)
	@$(foreach dir,src tests,$(MAKE) -C ${dir} $@;)

# file targets
src/%.cpp.o: src/%.cpp
	$(MAKE) -C src $(patsubst src/%,%,$@)

CRIO_IP = 10.0.0.11

deploy: ts_M1M3Thermal
	@echo '[SCP] $^'
	${co}scp $^ admin@${CRIO_IP}:
	@echo '[SCP] Bitfiles/NiFpga_M1M3SupportFPGA.lvbitx'
	${co}scp Bitfiles/NiFpga_M1M3SupportFPGA.lvbitx admin@${CRIO_IP}:Bitfiles

tests: tests/Makefile tests/*.cpp
	@${MAKE} -C tests

run_tests: tests
	@${MAKE} -C tests run

junit: tests
	@${MAKE} -C tests junit

doc:
	${co}doxygen Doxyfile
