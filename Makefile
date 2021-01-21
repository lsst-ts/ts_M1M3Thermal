include Makefile.inc

.PHONY: all clean deploy tests FORCE doc

# Add inputs and outputs from these tool invocations to the build variables 
#

# All Target
all: M1M3ThermalCsC m1m3tscli

src/libM1M3TS.a: FORCE
	$(MAKE) -C src libM1M3TS.a

M1M3ThermalCsC: src/m1m3thermalcsc.cpp.o src/libM1M3TS.a
	@echo '[LD ] $@'
	${co}$(CPP) $(LIBS_FLAGS) -o $@ $^ $(LIBS) ../ts_cRIOcpp/src/libcRIOcpp.a

m1m3tscli: src/m1m3tscli.cpp.o src/libM1M3TS.a
	@echo '[LD ] $@'
	${co}$(CPP) $(LIBS_FLAGS) -o $@ $^ $(LIBS) ../ts_cRIOcpp/src/libcRIOcpp.a -lreadline

# Other Targets
clean:
	@$(foreach file,M1M3ThermalCsC src/m1m3thermalcsc.cpp.o doc, echo '[RM ] ${file}'; $(RM) -r $(file);)
	@$(foreach dir,src tests,$(MAKE) -C ${dir} $@;)

# file targets
src/%.cpp.o: src/%.cpp
	$(MAKE) -C src $(patsubst src/%,%,$@)

CRIO_IP = 10.0.0.15

deploy: M1M3ThermalCsC
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
