/*
 * Simulated thermal FPGA class.
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

#include <cRIO/FPGA.h>
#include <cRIO/NiError.h>
#include <cRIO/ModbusBuffer.h>
#include <cRIO/ThermalILC.h>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Simulated Thermal FPGA. Simulates answers to FPGA functions.
 */
class SimulatedFPGA : public LSST::cRIO::FPGA, public LSST::cRIO::ThermalILC {
public:
    SimulatedFPGA(){};
    virtual ~SimulatedFPGA(){};
    void initialize() override{};
    void open() override{};
    void close() override{};
    void finalize() override{};
    void writeCommandFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
    void writeRequestFIFO(uint16_t* data, int32_t length, int32_t timeout) override;
    void readU16ResponseFIFO(uint16_t* data, int32_t length, int32_t timeout) override;
    void waitOnIrqs(uint32_t irqs, uint32_t timeout, uint32_t* triggered = NULL) override{};
    void ackIrqs(uint32_t irqs){};

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

private:
    LSST::cRIO::ModbusBuffer response;

    void _simulateModbus(uint16_t* data, size_t length);
};  // namespace TS

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST
