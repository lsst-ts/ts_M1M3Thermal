# define to "" to produce verbose output
ifndef VERBOSE
  co := @
  silence := --silence-errors
endif

c_opts := -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE

ifdef DEBUG
  c_opts += -g
  silence := --silence-errors
else
  c_opts += -O3
endif

CRIOCPP := ../ts_cRIOcpp/

PKG_CPPFLAGS := \
	$(shell pkg-config yaml-cpp --cflags $(silence)) \
	$(shell pkg-config spdlog --cflags $(silence)) \
	$(shell pkg-config fmt --cflags $(silence))

ifdef WITH_SAL_KAFKA
  SAL_CPPFLAGS += $(PKG_CPPFLAGS)  \
	-I${SAL_WORK_DIR}/MTVMS/cpp/src -I${SAL_WORK_DIR}/include -I${CRIOCPP} -I. \
	-I${SAL_HOME}/include -I${LSST_SDK_INSTALL}/include -I${SAL_HOME}/include -I${LSST_SAL_PREFIX}/include -I${LSST_SAL_PREFIX}/include/avro
  LIBS += $(PKG_LIBS) -ldl -lpthread -L/usr/lib64/boost${BOOST_RELEASE} -lboost_filesystem -lboost_iostreams \
	-lboost_program_options -lboost_system ../ts_cRIOcpp/lib/libcRIOcpp.a \
	-L${LSST_SAL_PREFIX}/lib -lcurl -lrdkafka++ -lrdkafka -lavrocpp -lavro -ljansson -lserdes++ -lserdes ${LSST_SAL_PREFIX}/lib/libSAL_MTM1M3TS.a -lstdc++fs
  LIBS_FLAGS += -L${LSST_SAL_PREFIX}/lib
else
  SAL_CPPFLAGS += $(PKG_CPPFLAGS) \
	-I${OSPL_HOME}/include -I${OSPL_HOME}/include/sys -I${OSPL_HOME}/include/dcps/C++/SACPP \
	-I${SAL_WORK_DIR}/MTM1M3TS/cpp/src -I${SAL_WORK_DIR}/include \
	-I${SAL_HOME}/include -I${LSST_SDK_INSTALL}/include
endif

PKG_LIBS := \
	$(shell pkg-config yaml-cpp --libs $(silence))

CPP := g++ -std=c++17 -Wall -fPIE ${c_opts}

ifdef SIMULATOR
  C := gcc -Wall ${c_opts}
  CPP += -pedantic -DSIMULATOR
  PKG_LIBS += $(shell pkg-config spdlog --libs $(silence))
else
  C := gcc -Wall -fmessage-length=0 ${c_opts}
  CPP += -fmessage-length=0
  # static linkage for cRIO binary
  PKG_LIBS += $(shell pkg-config spdlog --libs $(silence) | sed -E 's/-l([a-z0-9]*)/-l:lib\1.a/g')
endif

LIBS += $(PKG_LIBS) -ldl

ifdef WITH_SAL_KAFKA
  SAL_LIBS := ${LSST_SAL_PREFIX}/lib/libSAL_MTM1M3TS.a
else
  SAL_LIBS := -ldcpssacpp -ldcpsgapi -lddsuser -lddskernel \
	-lpthread -lddsserialization -lddsconfparser -lddsconf -lddsdatabase \
	-lddsutil -lddsos \
	${SAL_WORK_DIR}/lib/libSAL_MTM1M3TS.a
  LIBS_FLAGS += -L${OSPL_HOME}/lib -L${LSST_SDK_INSTALL}/lib
endif


VERSION := $(shell git describe --tags --dirty 2>/dev/null || echo "unknown:non-git")
GIT_HASH := $(shell git rev-parse HEAD 2>/dev/null || echo "unknown-non-git")
