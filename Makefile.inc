# define to "" to produce verbose output
ifndef VERBOSE
  co := @
endif

c_opts := -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE

ifdef DEBUG
  c_opts += -g
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
SAL_CPPFLAGS += $(shell pkg-config yaml-cpp --cflags) -I${OSPL_HOME}/include -I${OSPL_HOME}/include/sys -I${OSPL_HOME}/include/dcps/C++/SACPP -I${SAL_WORK_DIR}/MTM1M3TS/cpp/src -I${SAL_WORK_DIR}/include -I${SAL_HOME}/include -I${LSST_SDK_INSTALL}/include

CRIO_DIR ?= ../ts_cRIOcpp
CRIO_CFLAGS := -I${CRIO_DIR}/include

ifdef SIMULATOR
  LIBS := $(shell pkg-config yaml-cpp --libs) -ldl ${SAL_WORK_DIR}/lib/libSAL_MTM1M3TS.a -ldcpssacpp -ldcpsgapi -lddsuser -lddskernel -lpthread -lddsserialization -lddsconfparser -lddsconf -lddsdatabase -lddsutil -lddsos
else
  LIBS := $(shell pkg-config yaml-cpp --libs) -ldl ${SAL_WORK_DIR}/lib/libSAL_MTM1M3TS.a -lpthread
  SAL_LIBS := -ldcpssacpp -ldcpsgapi -lddsuser -lddskernel -lddsserialization -lddsconfparser -lddsconf -lddsdatabase -lddsutil -lddsos
endif

LIBS_FLAGS += -L"${SAL_WORK_DIR}/lib" -L"${OSPL_HOME}/lib" -L"${LSST_SDK_INSTALL}/lib"

GIT := $(shell command -v git 2> /dev/null)

ifndef GIT
   VERSION := "v0.0.0-None"
else
   VERSION := $(shell git describe --tags --dirty)
endif
