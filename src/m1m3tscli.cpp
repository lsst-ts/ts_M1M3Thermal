/*
 * Command line Thermal System client.
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

#ifdef SIMULATOR
#include <SimulatedFPGA.h>
#define FPGAClass SimulatedFPGA
#else
#include <ThermalFPGA.h>
#define FPGAClass ThermalFPGA
#endif

#include <cRIO/ThermalILC.h>
#include <cRIO/PrintILC.h>
#include <cRIO/FPGACliApp.h>
#include <cRIO/MPU.h>

#include <iostream>
#include <iomanip>

#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;

class M1M3TScli : public FPGACliApp {
public:
    M1M3TScli(const char* name, const char* description);

    int mpuRegisters(command_vec cmds);

protected:
    virtual FPGA* newFPGA(const char* dir) override;
    virtual ILCUnits getILCs(command_vec cmds) override;
};

class PrintThermalILC : public ThermalILC, public PrintILC {
public:
    PrintThermalILC(uint8_t bus) : ILC(bus), ThermalILC(bus), PrintILC(bus) {}

protected:
    void processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature, uint8_t fanRPM,
                              float absoluteTemperature) override;
};

class PrintTSFPGA : public FPGAClass {
public:
#ifdef SIMULATOR
    PrintTSFPGA() : SimulatedFPGA() {}
#else
    PrintTSFPGA() : ThermalFPGA() {}
#endif

    void writeMPUFIFO(MPU& mpu) override;
    void readMPUFIFO(MPU& mpu) override;
    void writeCommandFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
    void writeRequestFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
    void readU16ResponseFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
};

M1M3TScli::M1M3TScli(const char* name, const char* description) : FPGACliApp(name, description) {
    addCommand("mpu-registers", std::bind(&M1M3TScli::mpuRegisters, this, std::placeholders::_1), "si?",
               NEED_FPGA, "<mpu> <register>..", "Reads MPU given MPU registers");
    addILC(std::make_shared<PrintThermalILC>(1));

    addMPU("vfd", std::make_shared<MPU>(1, 100));
    addMPU("flow", std::make_shared<MPU>(2, 10));
}

int M1M3TScli::mpuRegisters(command_vec cmds) {
    std::shared_ptr<MPU> mpu = getMPU(cmds[0]);
    if (mpu == NULL) {
        std::cerr << "Invalid MPU device name " << cmds[0] << ". List of known devices: " << std::endl;
        printMPU();
        return -1;
    }
    mpu->clearCommanded();

    std::vector<uint16_t> registers;

    for (size_t i = 1; i < cmds.size(); i++) {
        registers.push_back(stoi(cmds[i], nullptr, 0));
    }

    for (auto r : registers) {
        mpu->readHoldingRegisters(r, 1);
        mpu->clear(true);
    }

    getFPGA()->mpuCommands(*mpu);

    for (auto r : registers) {
        uint16_t v = mpu->getRegister(r);
        std::cout << fmt::format("{0:>5d} ({0:04x}): {1:d} ({1:x})", r, v) << std::endl;
    }

    return 0;
}

FPGA* M1M3TScli::newFPGA(const char* dir) { return new PrintTSFPGA(); }

ILCUnits M1M3TScli::getILCs(command_vec cmds) {
    ILCUnits units;
    int ret = -2;

    for (auto c : cmds) {
        try {
            int address = std::stoi(c);
            if (address <= 0 || address > NUM_TS_ILC) {
                std::cerr << "Invalid address " << c << std::endl;
                ret = -1;
                continue;
            }
            units.push_back(ILCUnit(getILC(0), address));
        } catch (std::logic_error& e) {
            std::cerr << "Non-numeric address: " << c << std::endl;
            ret = -1;
        }
        ret = 0;
        return units;
    }

    if (ret == -2) {
        std::cout << "Info for all ILC" << std::endl;
        for (int i = 1; i <= NUM_TS_ILC; i++) {
            units.push_back(ILCUnit(getILC(0), i));
        }
        ret = 0;
    }
    return units;
}

void PrintThermalILC::processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature,
                                           uint8_t fanRPM, float absoluteTemperature) {
    printBusAddress(address);
    std::cout << "Thermal status: " << std::to_string(status) << std::endl
              << "Differential temperature: " << std::to_string(differentialTemperature) << std::endl
              << "Fan RPM: " << std::to_string(fanRPM) << std::endl;
}

M1M3TScli cli("M1M3TS", "M1M3 Thermal System Command Line Interface");

void _printBufferU8(std::string prefix, uint8_t* buf, size_t len) {
    if (cli.getDebugLevel() == 0) {
        return;
    }

    std::cout << prefix;
    for (size_t i = 0; i < len; i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(buf[i]) << " ";
    }
    std::cout << std::endl;
}

void _printBufferU16(std::string prefix, uint16_t* buf, size_t len) {
    if (cli.getDebugLevel() == 0) {
        return;
    }

    std::cout << prefix;
    for (size_t i = 0; i < len; i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(4) << buf[i] << " ";
    }
    std::cout << std::endl;
}

void PrintTSFPGA::writeMPUFIFO(MPU& mpu) {
    _printBufferU8("M> ", mpu.getCommands(), mpu.getCommandVector().size());
    FPGAClass::writeMPUFIFO(mpu);
}

void PrintTSFPGA::readMPUFIFO(MPU& mpu) {
    FPGAClass::readMPUFIFO(mpu);
    _printBufferU16("M< ", mpu.getBuffer(), mpu.getLength());
}

void PrintTSFPGA::writeCommandFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    _printBufferU16("C> ", data, length);
    FPGAClass::writeCommandFIFO(data, length, timeout);
}

void PrintTSFPGA::writeRequestFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    _printBufferU16("R> ", data, length);
    FPGAClass::writeRequestFIFO(data, length, timeout);
}

void PrintTSFPGA::readU16ResponseFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    FPGAClass::readU16ResponseFIFO(data, length, timeout);
    _printBufferU16("R< ", data, length);
}

int main(int argc, char* const argv[]) { return cli.run(argc, argv); }
