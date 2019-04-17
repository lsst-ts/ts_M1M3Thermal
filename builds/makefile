###############################################################################
#  This file is part of MTM1M3TS.
# 
#  Developed for the LSST Telescope and Site Systems.
#  This product includes software developed by the LSST Project
#  (https://www.lsst.org).
#  See the COPYRIGHT file at the top-level directory of this distribution
#  for details of code ownership.
# 
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.
###############################################################################

CXX = g++

CXXFLAGS = \
	-std=c++0x \
	-MMD \
	-MP \
	-MF"$(@:%.o=%.d)" \
	-MT"$(@)" \
	-c \
	-Wall \
	-fmessage-length=0 \
	-O3 \
	-m64 \
	-D_REENTRANT \
	-fPIC \
	-I"../include" \
	-I"$(OSPL_HOME)/include" \
	-I"$(OSPL_HOME)/include/sys" \
	-I"$(OSPL_HOME)/include/dcps/C++/SACPP" \
	-I"$(SAL_HOME)/include" \
	-I"$(SAL_WORK_DIR)/include" \
	-I"$(SAL_WORK_DIR)/MTM1M3TS/cpp" \
	-I"$(SAL_WORK_DIR)/MTM1M3TS/cpp/src" \
	-I"$(SAL_WORK_DIR)/MTM1M3SS/cpp" \
	-I"$(SAL_WORK_DIR)/MTM1M3SS/cpp/src"

LDFLAGS = \
	-L"../lib" \
	-L"$(OSPL_HOME)/lib" \
	-L"$(SAL_HOME)/lib" \
	-L"$(SAL_WORK_DIR)/lib"

LIBS = \
	-ldl \
	-lpthread \
	-lyaml-cpp \
	-lSAL_MTM1M3TS \
	-lsacpp_MTM1M3TS_types \
	-lSAL_MTM1M3SS \
	-lsacpp_MTM1M3SS_types \
	-ldcpssacpp \
	-ldcpsgapi \
	-lddsuser \
	-lddskernel \
	-lddsserialization \
	-lddsconfparser \
	-lddsdatabase \
	-lddsutil \
	-lddsos \
	-lddsconf

# For each *.cpp file place an entry here
# Example: ./src/Settings/Settings.cpp is Settings/Settings
APP := \
	Domain/Functions \
	Domain/Limits \
	InterlockSystem/InterlockSystem \
	MainSystem/Commands \
	MainSystem/Controller \
	MainSystem/MainSystem \
	Settings/Settings \
	Threads/ControllerThread \
	Threads/IThread \
	Threads/SubscriberThread \
	Utility/Logger \
	Utility/whereami

APP_CPP_SRCS := $(APP:%=../src/%.cpp)
APP_OBJS := $(APP:%=./%.o)
APP_CPP_DEPS := $(APP:%=./%.d)

# For each *.cpp file place an entry here
# Example: ./tests/SettingsTests.cpp is SettingsTests
TEST := \
	MainSystemTests \
	InterlockSystemTests \
	SettingsTests \
	FunctionsTests \
	LimitsTests

TEST_CPP_SRCS := $(TEST:%=../tests/%.cpp)
TEST_OBJS := $(TEST:%=./tests/%.o)
TEST_CPP_DEPS := $(TEST:%=./tests/%.d)

-include $(APP_CPP_DEPS)
-include $(TEST_CPP_DEPS)

all : \
	clean \
	app \
	tests
	./ts_M1M3ThermalUnitTests

app : \
	Main.o \
	$(APP_OBJS)
	$(CXX) $(LDFLAGS) -o ts_M1M3Thermal ./Main.o $(APP_OBJS) $(LIBS)

Main.o : ../src/Main.cpp
	$(CXX) $(CXXFLAGS) -o "Main.o" ../src/Main.cpp

# For each directory in the src folder replicate one of these
# Also make sure to create that directory in ./builds

Domain/%.o : ../src/Domain/%.cpp
	$(CXX) $(CXXFLAGS) -o "$@" "$<"

InterlockSystem/%.o : ../src/InterlockSystem/%.cpp
	$(CXX) $(CXXFLAGS) -o "$@" "$<"

MainSystem/%.o : ../src/MainSystem/%.cpp
	$(CXX) $(CXXFLAGS) -o "$@" "$<"

Settings/%.o : ../src/Settings/%.cpp
	$(CXX) $(CXXFLAGS) -o "$@" "$<"

Threads/%.o : ../src/Threads/%.cpp
	$(CXX) $(CXXFLAGS) -o "$@" "$<"

Utility/%.o : ../src/Utility/%.cpp
	$(CXX) $(CXXFLAGS) -o "$@" "$<"

tests : \
	Tests.o \
	$(APP_OBJS) \
	$(TEST_OBJS)
	$(CXX) $(LDFLAGS) -o ts_M1M3ThermalUnitTests ./Tests.o $(APP_OBJS) $(TEST_OBJS) $(LIBS)

Tests.o : ../tests/Tests.cpp
	$(CXX) $(CXXFLAGS) -o "Tests.o" ../tests/Tests.cpp

tests/%.o : ../tests/%.cpp
	$(CXX) $(CXXFLAGS) -o "$@" "$<"

clean :
	-rm \
	ts_M1M3Thermal \
	ts_M1M3ThermalUnitTests \
	./Main.o \
	./Main.d \
	./Tests.o \
	./Tests.d \
	$(APP_OBJS) \
	$(APP_CPP_DEPS) \
	$(TEST_OBJS) \
	$(TEST_CPP_DEPS)

.PHONY : clean all