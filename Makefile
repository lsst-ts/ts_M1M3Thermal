include Makefile.inc

.PHONY: all clean deploy tests FORCE doc clang-format simulator ipk

# Add inputs and outputs from these tool invocations to the build variables 
#

# All Target
all: ts-M1M3thermald m1m3tscli

src/libM1M3TS.a: FORCE
	$(MAKE) -C src libM1M3TS.a

ts-M1M3thermald: src/ts-M1M3thermald.cpp.o src/libM1M3TS.a
	@echo '[LD ] $@'
	${co}$(CPP) $(LIBS_FLAGS) -o $@ $^  ../ts_cRIOcpp/lib/libcRIOcpp.a $(LIBS) $(SAL_LIBS) $(shell pkg-config --libs readline $(silence)) -lreadline


m1m3tscli: src/m1m3tscli.cpp.o src/libM1M3TS.a
	@echo '[LD ] $@'
	${co}$(CPP) $(LIBS_FLAGS) -o $@ $^  ../ts_cRIOcpp/lib/libcRIOcpp.a $(LIBS) $(shell pkg-config --libs readline $(silence)) -lreadline

# Other Targets
clean:
	@$(foreach file,M1M3ThermalCsC src/m1m3thermalcsc.cpp.o, echo '[RM ] ${file}'; $(RM) -r $(file);)
	@$(foreach dir,src tests,$(MAKE) -C ${dir} $@;)

# file targets
src/%.cpp.o: src/%.cpp
	$(MAKE) -C src $(patsubst src/%,%,$@)

CRIO_IP = 10.0.0.15

deploy: M1M3ThermalCsC
	@echo '[SCP] $^'
	${co}scp $^ admin@${CRIO_IP}:
	@echo '[SCP] Bitfiles/NiFpga_ts_M1M3ThermalFPGA.lvbitx
	${co}scp Bitfiles/NiFpga_ts_M1M3ThermalFPGA.lvbitx admin@${CRIO_IP}:Bitfiles

tests: tests/Makefile tests/*.cpp src/libM1M3TS.a
	@${MAKE} -C tests

run_tests: tests
	@${MAKE} -C tests run

junit: tests
	@${MAKE} -C tests junit

doc:
	${co}doxygen Doxyfile

simulator:
	@${MAKE} SIMULATOR=1 DEBUG=1

clang-format:
	$(MAKE) -C src clang-format

ipk: ts-M1M3thermal_${VERSION}_x64.ipk

TS_DDSCONFIG=../ts_ddsconfig

ifdef WITH_SAL_KAFKA
  ts-M1M3thermal_$(VERSION)_x64.ipk: ts-M1M3thermald m1m3tscli
	@echo '[MK ] ipk $@'
	${co}mkdir -p ipk/data/usr/sbin
	${co}mkdir -p ipk/data/etc/init.d
	${co}mkdir -p ipk/data/etc/default
	${co}mkdir -p ipk/data/var/lib/M1M3TS
	${co}mkdir -p ipk/control
	${co}cp ts-M1M3thermald ipk/data/usr/sbin/ts-M1M3thermald
	${co}cp m1m3tscli ipk/data/usr/sbin/m1m3tscli
	${co}cp init ipk/data/etc/init.d/ts-M1M3thermal
	${co}cp default_ts-M1M3thermal ipk/data/etc/default/ts-M1M3thermal
	${co}cp -r SettingFiles/* ipk/data/var/lib/M1M3TS
	${co}cp -r Bitfiles/* ipk/data/var/lib/M1M3TS
	${co}sed s?@VERSION@?$(VERSION)?g control.ipk.in > ipk/control/control
	${co}cp postinst prerm postrm ipk/control
	${co}echo -e "/etc/default/ts-M1M3thermal\n/var/lib/M1M3TS/ospl-embedded-shmem.xml\n/var/lib/M1M3TS/QoS.xml" > ipk/control/conffiles
	${co}find SettingFiles -name '*.xml' -o -name '*.csv' | sed 's#^SettingFiles#/var/lib/M1M3TS#' >> ipk/control/conffiles
	${co}echo "2.0" > ipk/debian-binary
	${co}tar czf ipk/data.tar.gz -P --transform "s#^ipk/data#.#" --owner=0 --group=0 ipk/data
	${co}tar czf ipk/control.tar.gz -P --transform "s#^ipk/control#.#" --owner=0 --group=0 ipk/control
	${co}ar r $@ ipk/control.tar.gz ipk/data.tar.gz ipk/debian-binary
else
  ts-M1M3thermal_$(VERSION)_x64.ipk: ts-M1M3thermald m1m3tscli
	@echo '[MK ] ipk $@'
	${co}mkdir -p ipk/data/usr/sbin
	${co}mkdir -p ipk/data/etc/init.d
	${co}mkdir -p ipk/data/etc/default
	${co}mkdir -p ipk/data/var/lib/M1M3TS
	${co}mkdir -p ipk/control
	${co}cp ts-M1M3thermald ipk/data/usr/sbin/ts-M1M3thermald
	${co}cp m1m3tscli ipk/data/usr/sbin/m1m3tscli
	${co}cp init ipk/data/etc/init.d/ts-M1M3thermal
	${co}cp default_ts-M1M3thermal ipk/data/etc/default/ts-M1M3thermal
	${co}cp -r ${TS_DDSCONFIG}/python/lsst/ts/ddsconfig/data/config/ospl-embedded-shmem.xml ipk/data/var/lib/M1M3TS || echo "Cannot find ${TS_DDSCONFIG} ospl-embedded-shmem.xml - check it out?"
	${co}cp -r ${TS_DDSCONFIG}/python/lsst/ts/ddsconfig/data/qos/QoS.xml ipk/data/var/lib/M1M3TS || echo "Cannot find ${TS_DDSCONFIG} QoS.xml - check it out?"
	${co}cp -r SettingFiles/* ipk/data/var/lib/M1M3TS
	${co}cp -r Bitfiles/* ipk/data/var/lib/M1M3TS
	${co}sed s?@VERSION@?$(VERSION)?g control.ipk.in > ipk/control/control
	${co}cp postinst prerm postrm ipk/control
	${co}echo -e "/etc/default/ts-M1M3thermal\n/var/lib/M1M3TS/ospl-embedded-shmem.xml\n/var/lib/M1M3TS/QoS.xml" > ipk/control/conffiles
	${co}find SettingFiles -name '*.xml' -o -name '*.csv' | sed 's#^SettingFiles#/var/lib/M1M3TS#' >> ipk/control/conffiles
	${co}echo "2.0" > ipk/debian-binary
	${co}tar czf ipk/data.tar.gz -P --transform "s#^ipk/data#.#" --owner=0 --group=0 ipk/data
	${co}tar czf ipk/control.tar.gz -P --transform "s#^ipk/control#.#" --owner=0 --group=0 ipk/control
	${co}ar r $@ ipk/control.tar.gz ipk/data.tar.gz ipk/debian-binary
endif
  
