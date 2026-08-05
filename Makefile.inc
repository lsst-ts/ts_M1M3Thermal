# define to "" to produce verbose output
#define VERBOSE ""

ifndef VERBOSE
  co := @
  silence := --silence-errors
endif

# change as needed
c_opts := -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE -DSPDLOG_FMT_EXTERNAL

ifdef DEBUG
  c_opts += -g
  silence := --silence-errors
else
  c_opts += -O3
endif

CRIOCPP := ../ts_cRIOcpp/

PKG_CPPFLAGS := $(shell pkg-config yaml-cpp spdlog fmt avro-cpp --cflags $(silence))

SAL_CPPFLAGS += $(PKG_CPPFLAGS)  \
	-I${SAL_WORK_DIR}/MTM1M3TS/cpp/src \
	-I${SAL_WORK_DIR}/include -I${CRIOCPP} -I. \
	-I${SAL_HOME}/include -I${LSST_SDK_INSTALL}/include -I${SAL_HOME}/include -I${LSST_SAL_PREFIX}/include -I${LSST_SAL_PREFIX}/include/avro
LIBSCHEMAREGISTRY_VCPKG_LIB ?= /opt/vcpkg/installed/x64-linux/lib
LIBSCHEMAREGISTRY_LDLIBS = \
    -Wl,--start-group \
    $(LSST_SAL_PREFIX)/lib/libschemaregistry.a \
    $(wildcard $(LIBSCHEMAREGISTRY_VCPKG_LIB)/libabsl_*.a) \
    $(LIBSCHEMAREGISTRY_VCPKG_LIB)/libcpr.a \
    $(LIBSCHEMAREGISTRY_VCPKG_LIB)/libcurl.a \
    $(LIBSCHEMAREGISTRY_VCPKG_LIB)/libssl.a \
    $(LIBSCHEMAREGISTRY_VCPKG_LIB)/libcrypto.a \
    $(LIBSCHEMAREGISTRY_VCPKG_LIB)/libz.a \
    $(LIBSCHEMAREGISTRY_VCPKG_LIB)/libbz2.a \
    $(LIBSCHEMAREGISTRY_VCPKG_LIB)/liblzma.a \
    $(LIBSCHEMAREGISTRY_VCPKG_LIB)/libzstd.a \
    -Wl,--end-group
LIBS += $(PKG_LIBS) -ldl -lpthread -L/usr/lib64/boost${BOOST_RELEASE} -lboost_filesystem -lboost_iostreams \
	-lboost_program_options -lboost_system \
	${SAL_WORK_DIR}/lib/libSAL_MTM1M3TS.a \
	-L${LSST_SAL_PREFIX}/lib -L${SAL_WORK_DIR}/lib $(LIBSCHEMAREGISTRY_LDLIBS) -lcurl -lrdkafka++ -lrdkafka -lavrocpp -lavro -ljansson -lsasl2
LIBS_FLAGS += -L${LSST_SAL_PREFIX}/lib

PKG_LIBS := $(shell pkg-config yaml-cpp spdlog fmt --libs $(silence))

CPP := g++ -std=c++20 -Wall -fPIE ${c_opts}

# compile x86 simulator or real cRIO stuff
ifdef SIMULATOR
  C := gcc -Wall ${c_opts}
  CPP += -DSIMULATOR
  PKG_LIBS += $(shell pkg-config spdlog avro-cpp --libs $(silence)) -L /usr/local/lib
else
  C := gcc -Wall -fmessage-length=0 ${c_opts}
  CPP += -fmessage-length=0
  PKG_LIBS += $(shell pkg-config spdlog --libs $(silence))
endif

LIBS += $(PKG_LIBS) -ldl -lpthread -L/usr/lib64/boost${BOOST_RELEASE} -lboost_filesystem -lboost_iostreams \
	-lboost_program_options -lboost_system \
	-L${LSST_SAL_PREFIX}/lib -lcurl -lrdkafka++ -lrdkafka -lavrocpp -lavro -ljansson

VERSION := $(shell git describe --tags --dirty 2>/dev/null || echo "unknown:non-git")
GIT_HASH := $(shell git rev-parse HEAD 2>/dev/null || echo "unknown-non-git")
