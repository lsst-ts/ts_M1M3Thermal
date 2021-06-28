/*
 * This file is part of LSST cRIOcpp test suite. Tests ILC generic functions.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software Systems.
 * This product includes software developed by the Vera C.Rubin Observatory Project
 * (https://www.lsst.org). See the COPYRIGHT file at the top-level directory of
 * this distribution for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>

#include <SimulatedFPGA.h>
#include <cRIO/ThermalILC.h>

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;

class TestILC : public ThermalILC {
protected:
    void processServerID(uint8_t address, uint64_t uniqueID, uint8_t ilcAppType, uint8_t networkNodeType,
                         uint8_t ilcSelectedOptions, uint8_t networkNodeOptions, uint8_t majorRev,
                         uint8_t minorRev, std::string firmwareName) override;

    void processServerStatus(uint8_t address, uint8_t mode, uint16_t status, uint16_t faults) override;

    void processChangeILCMode(uint8_t address, uint16_t mode) override;

    void processSetTempILCAddress(uint8_t address, uint8_t newAddress) override;

    void processResetServer(uint8_t address) override;

    void processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature, uint8_t fanRPM,
                              float absoluteTemperature) override;
};

void TestILC::processServerID(uint8_t address, uint64_t uniqueID, uint8_t ilcAppType, uint8_t networkNodeType,
                              uint8_t ilcSelectedOptions, uint8_t networkNodeOptions, uint8_t majorRev,
                              uint8_t minorRev, std::string firmwareName) {
    REQUIRE(address == 16);
    REQUIRE(uniqueID == 0x040302010000 + (static_cast<uint64_t>(address) << 40));
    REQUIRE(ilcAppType == 0x02);
    REQUIRE(networkNodeType == 0x02);
    REQUIRE(ilcSelectedOptions == 0x02);
    REQUIRE(networkNodeOptions == 0x00);
    REQUIRE(majorRev == 1);
    REQUIRE(minorRev == 2);
    REQUIRE(firmwareName == "Test Thermal ILC");
}

void TestILC::processServerStatus(uint8_t address, uint8_t mode, uint16_t status, uint16_t faults) {
    REQUIRE(mode == 0);
}

void TestILC::processChangeILCMode(uint8_t address, uint16_t mode) { REQUIRE(address == 123); }

void TestILC::processSetTempILCAddress(uint8_t address, uint8_t newAddress) { REQUIRE(newAddress == 167); }

void TestILC::processResetServer(uint8_t address) { REQUIRE(address == 198); }

void TestILC::processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature,
                                   uint8_t fanRPM, float absoluteTemperature) {
    REQUIRE(address == 56);
}

TEST_CASE("Test simulated FPGA responses", "[SimulatedFPGA]") {
    SimulatedFPGA simulated;
    TestILC testILC;

    testILC.reportServerID(16);

    simulated.ilcCommands(testILC);
}
