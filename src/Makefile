include ../Makefile.inc

m1m3tscli: m1m3tscli.cpp.o
	@echo '[LD ] $@'
	${co}$(CPP) -o $@ $^ ../../ts_cRIOcpp/src/libcRIOcpp.a -lreadline


%.cpp.o: %.cpp.d
	@echo '[CPP] $(patsubst %.d,%,$<)'
	${co}$(CPP) $(CRIO_CFLAGS) -c -o $@ $(patsubst %.d,%,$<)

%.cpp.d: %.cpp
	@echo '[DEP] $<'
	${co}$(CPP) $(CRIO_CFLAGS) -M $< -MF $@ -MT '$(patsubst %.cpp,%.o,$<) $@'
