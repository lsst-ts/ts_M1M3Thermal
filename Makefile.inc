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

# compile x86 simulator or real cRIO stuff
ifdef SIMULATOR
  C := gcc -Wall ${c_opts}
  CPP := g++ -std=c++14 -pedantic -Wall ${c_opts} -DSIMULATOR -lpthread
else
  C := gcc -Wall -fmessage-length=0 ${c_opts}
  CPP := g++ -std=c++14 -Wall -fmessage-length=0 ${c_opts} -ldl -lpthread
endif

BOOST_CPPFLAGS += -I/usr/include/boost169
PKG_CPPFLAGS := $(shell pkg-config yaml-cpp spdlog --cflags $(silence))
SAL_CPPFLAGS += $(PKG_CPPFLAGS) -I${OSPL_HOME}/include -I${OSPL_HOME}/include/sys -I${OSPL_HOME}/include/dcps/C++/SACPP -I${SAL_WORK_DIR}/MTM1M3TS/cpp/src -I${SAL_WORK_DIR}/include -I${SAL_HOME}/include -I${LSST_SDK_INSTALL}/include

CRIO_DIR ?= ../ts_cRIOcpp
CRIO_CFLAGS := -I${CRIO_DIR}/include

PKG_LIBS := $(shell pkg-config yaml-cpp spdlog --libs $(silence))

ifdef SIMULATOR
  LIBS := $(PKG_LIBS) -ldl ${SAL_WORK_DIR}/lib/libSAL_MTM1M3TS.a -ldcpssacpp -ldcpsgapi -lddsuser -lddskernel -lpthread -lddsserialization -lddsconfparser -lddsconf -lddsdatabase -lddsutil -lddsos
else
  LIBS := $(PKG_LIBS) ${SAL_WORK_DIR}/lib/libSAL_MTM1M3TS.a -lpthread
  SAL_LIBS := -ldcpssacpp -ldcpsgapi -lddsuser -lddskernel -lddsserialization -lddsconfparser -lddsconf -lddsdatabase -lddsutil -lddsos
endif

LIBS_FLAGS += -L"${SAL_WORK_DIR}/lib" -L"${OSPL_HOME}/lib" -L"${LSST_SDK_INSTALL}/lib"

VERSION := $(shell git describe --tags --always --dirty 2>/dev/null || echo "unknown:non-git")
GIT_HASH := $(shell git rev-parse HEAD 2>/dev/null || echo "unknown-non-git")
