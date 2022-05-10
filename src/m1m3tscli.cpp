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

#include <MPU/FlowMeter.h>

#include <iostream>
#include <iomanip>
#include <memory>

#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;

class M1M3TScli : public FPGACliApp {
public:
    M1M3TScli(const char* name, const char* description);

    int mpuRegisters(command_vec cmds);
    int printFlowMeter(command_vec cmds);
    int mixingValve(command_vec cmds);
    int fcuOnOff(command_vec cmds);
    int pumpOnOff(command_vec cmds);
    int thermalDemand(command_vec cmds);
    int glycolTemperature(command_vec cmds);
    int glycolDebug(command_vec cmds);
    int slot4(command_vec);

protected:
    virtual FPGA* newFPGA(const char* dir) override;
    virtual ILCUnits getILCs(command_vec cmds) override;

private:
    std::shared_ptr<FlowMeter> flowMeter;
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
    void readU8ResponseFIFO(uint8_t* data, size_t length, uint32_t timeout) override;
    void readU16ResponseFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
};

M1M3TScli::M1M3TScli(const char* name, const char* description) : FPGACliApp(name, description) {
    addCommand("mpu-registers", std::bind(&M1M3TScli::mpuRegisters, this, std::placeholders::_1), "SI?",
               NEED_FPGA, "<mpu> <register>..", "Reads MPU given MPU registers");
    addCommand("flow", std::bind(&M1M3TScli::printFlowMeter, this, std::placeholders::_1), "", NEED_FPGA,
               NULL, "Reads FlowMeter values");
    addCommand("mixing-valve", std::bind(&M1M3TScli::mixingValve, this, std::placeholders::_1), "d",
               NEED_FPGA, "[valve position (mA)]", "Reads and sets mixing valve position");
    addCommand("fcu-on", std::bind(&M1M3TScli::fcuOnOff, this, std::placeholders::_1), "b", NEED_FPGA,
               "[on|off]", "Command FCU power on/off");
    addCommand("pump-on", std::bind(&M1M3TScli::pumpOnOff, this, std::placeholders::_1), "b", NEED_FPGA,
               "[on|off]", "Command coolant pump on/off");

    addCommand("thermal-demand", std::bind(&M1M3TScli::thermalDemand, this, std::placeholders::_1), "iis?",
               NEED_FPGA, "<heater PWM> <fan RPM> <ILC..>", "Sets FCU heater and fan");
    addCommand("slot4", std::bind(&M1M3TScli::slot4, this, std::placeholders::_1), "", NEED_FPGA, NULL,
               "Reads slot 4 inputs");

    addILCCommand(
            "thermal-status",
            [](ILCUnit u) {
                std::dynamic_pointer_cast<PrintThermalILC>(u.first)->reportThermalStatus(u.second);
            },
            "Report thermal status");

    addCommand("glycol-temperature", std::bind(&M1M3TScli::glycolTemperature, this, std::placeholders::_1),
               "", NEED_FPGA, NULL, "Primts glycol temperature values");
    addCommand("glycol-debug", std::bind(&M1M3TScli::glycolDebug, this, std::placeholders::_1), "", NEED_FPGA,
               NULL, "Output last Glycol thermocouple line");

    addILC(std::make_shared<PrintThermalILC>(1));

    addMPU("vfd", std::make_shared<MPU>(1, 100));

    flowMeter = std::make_shared<FlowMeter>(2, 1);
    addMPU("flow", flowMeter);
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
        mpu->readHoldingRegisters(r, 1, 255);
        mpu->clear(true);
    }

    getFPGA()->mpuCommands(*mpu);

    for (auto r : registers) {
        uint16_t v = mpu->getRegister(r);
        std::cout << fmt::format("{0:>5d} ({0:04x}): {1:d} ({1:x})", r, v) << std::endl;
    }

    return 0;
}

int M1M3TScli::printFlowMeter(command_vec cmds) {
    flowMeter->clearCommanded();

    flowMeter->poll();

    getFPGA()->mpuCommands(*flowMeter);

    std::cout << std::setfill(' ') << std::setw(20) << "Signal Strength: " << flowMeter->getSignalStrength()
              << std::endl
              << std::setw(20) << "Flow Rate: " << flowMeter->getFlowRate() << std::endl
              << std::setw(20) << "Net Totalizer: " << flowMeter->getNetTotalizer() << std::endl
              << std::setw(20) << "Positive Totalizer: " << flowMeter->getPositiveTotalizer() << std::endl
              << std::setw(20) << "Negative Totalizer: " << flowMeter->getNegativeTotalizer() << std::endl
              << std::setw(20) << "Temperature 1: " << flowMeter->getTemperature1() << " \u00b0C" << std::endl
              << std::setw(20) << "Temperature 2: " << flowMeter->getTemperature2() << " \u00b0C"
              << std::endl;
    return 0;
}

int M1M3TScli::mixingValve(command_vec cmds) {
    if (cmds.size() == 1) {
        dynamic_cast<IFPGA*>(getFPGA())->setMixingValvePosition(std::stof(cmds[0]) / 1000.0f);
    }
    std::cout << "Mixing valve: " << std::fixed << std::setprecision(3)
              << dynamic_cast<IFPGA*>(getFPGA())->getMixingValvePosition() << " V" << std::endl;
    return 0;
}

int M1M3TScli::fcuOnOff(command_vec cmds) {
    if (cmds.size() == 1) {
        dynamic_cast<IFPGA*>(getFPGA())->setFCUPower(onOff(cmds[0]));
        std::cout << "Turned FCU power " << cmds[0] << std::endl;
    }
    return 0;
}

int M1M3TScli::pumpOnOff(command_vec cmds) {
    if (cmds.size() == 1) {
        dynamic_cast<IFPGA*>(getFPGA())->setPumpPower(onOff(cmds[0]));
        std::cout << "Turned pump " << cmds[0] << std::endl;
    }
    return 0;
}

FPGA* M1M3TScli::newFPGA(const char* dir) { return new PrintTSFPGA(); }

int M1M3TScli::thermalDemand(command_vec cmds) {
    uint8_t heater = std::stoi(cmds[0]);
    uint8_t fan = std::stoi(cmds[1]);
    cmds.erase(cmds.begin(), cmds.begin() + 2);

    clearILCs();
    ILCUnits ilcs = getILCs(cmds);
    for (auto u : ilcs) {
        std::dynamic_pointer_cast<PrintThermalILC>(u.first)->setThermalDemand(u.second, heater, fan);
    }
    getFPGA()->ilcCommands(*getILC(0));
    return 0;
}

int M1M3TScli::glycolTemperature(command_vec) {
    getFPGA()->writeRequestFIFO(FPGAAddress::GLYCOLTEMP_TEMPERATURES, 0);

    float temp[8];
    dynamic_cast<IFPGA*>(getFPGA())->readSGLResponseFIFO(temp, 8, 150);

    const char* names[8] = {"Above Mirror",          "Inside Cell 1",        "Inside Cell 2",
                            "Inside Cell 3",         "MTA Coolant Supply",   "MTA Coolant Return",
                            "Mirror Coolant Supply", "Mirror Coolant Return"};
    for (int i = 0; i < 8; i++) {
        std::cout << std::right << std::setw(30) << names[i] << ": " << std::setw(6) << std::fixed
                  << std::setprecision(2) << temp[i] << std::endl;
    }
    return 0;
}

int M1M3TScli::glycolDebug(command_vec) {
    std::vector<int> addrs;

    auto callAddr = [this](int a) {
        getFPGA()->writeRequestFIFO(a, 0);
        uint16_t len;
        dynamic_cast<IFPGA*>(getFPGA())->readU8ResponseFIFO(reinterpret_cast<uint8_t*>(&len), 2, 150);
        len = ntohs(len);
        if (len > 0) {
            uint8_t data[len + 1];
            dynamic_cast<IFPGA*>(getFPGA())->readU8ResponseFIFO(data, len, 10);

            data[len] = '\0';
            std::cout << "String out: " << data << std::endl;
        }
    };

    callAddr(FPGAAddress::GLYCOLTEMP_LAST_LINE);
    callAddr(FPGAAddress::GLYCOLTEMP_DEBUG);

    return 0;
}

int M1M3TScli::slot4(command_vec) {
    uint32_t dis = dynamic_cast<IFPGA*>(getFPGA())->getSlot4DIs();
    std::cout << "Slot4: 0x" << std::hex << std::setfill('0') << std::setw(4) << dis << std::endl
              << std::setfill(' ') << std::endl;

    const char* names[32] = {
            "PS 14 Status",                   // DI0
            "PS 15 Status",                   // DI1
            "PS 16 Status",                   // DI2
            "Ctrls Redundancy Status",        // DI3
            "Fan Coils Diffuser Status",      // DI4
            "AC Power CB15 Status",           // DI5
            "Utility outlet CB18 Status",     // DI6
            "Coolant pump OL status",         // DI7
            NULL,                             // DI8
            NULL,                             // DI9
            NULL,                             // DI10
            NULL,                             // DI11
            NULL,                             // DI12
            NULL,                             // DI13
            NULL,                             // DI14
            NULL,                             // DI15
            "FC heaters off interlock",       // DI16
            "Coolant pump off interlock",     // DI17
            "GIS HB loss interlock",          // DI18
            "mixing valve closed interlock",  // DI19
            "Support System HB loss",         // DI20
            "Cell door open interlock",       // DI21
            "GIS earthquake interlock",       // DI22
            "Coolant pump e-stop interlock",  // DI23
            "Cabinet Over Temp interlock",    // DI24
            NULL,                             // DI25
            NULL,                             // DI26
            NULL,                             // DI27
            NULL,                             // DI28
            NULL,                             // DI29
            NULL,                             // DI30
            NULL,                             // DI31
    };

    uint32_t b = 1;

    for (int i = 0; i < 32; i++, b <<= 1) {
        if (names[i]) {
            std::cout << std::right << std::setw(30) << names[i] << ": " << (dis & b ? "On" : "Off")
                      << std::endl;
        }
    }

    std::cout << std::endl;

    return 0;
}

ILCUnits M1M3TScli::getILCs(command_vec cmds) {
    ILCUnits units;
    int ret = -2;

    for (auto c : cmds) {
        size_t range = c.find('-');
        if (range != std::string::npos) {
            int start = std::stoi(c.substr(0, range));
            int end = std::stoi(c.substr(range + 1));
            for (int address = start; address <= end; address++) {
                units.push_back(ILCUnit(getILC(0), address));
            }
        } else {
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
        }
        ret = 0;
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
    std::cout << "Thermal status: 0x" << std::setfill('0') << std::setw(2) << std::hex
              << static_cast<int>(status) << std::endl
              << "Differential temperature: " << std::to_string(differentialTemperature) << std::endl
              << "Fan RPM: " << std::to_string(fanRPM) << std::endl
              << "Absolute temperature: " << std::to_string(absoluteTemperature) << std::endl;
}

M1M3TScli cli("M1M3TS", "M1M3 Thermal System Command Line Interface");

void _printBufferU8(std::string prefix, uint8_t* buf, size_t len) {
    if (cli.getDebugLevel() == 0) {
        return;
    }

    std::cout << prefix;

    CliApp::printHexBuffer(buf, len);

    std::cout << std::endl;
}

void _printBufferU16(std::string prefix, uint16_t* buf, size_t len) {
    if (cli.getDebugLevel() == 0) {
        return;
    }

    std::cout << prefix;

    CliApp::printHexBuffer(buf, len);

    if (cli.getDebugLevel() > 1 && len > 1) {
        std::cout << std::endl << prefix;
        CliApp::printDecodedBuffer(buf, len);
    }
    std::cout << std::endl;
}

void PrintTSFPGA::writeMPUFIFO(MPU& mpu) {
    _printBufferU8("M>", mpu.getCommands(), mpu.getCommandVector().size());
    FPGAClass::writeMPUFIFO(mpu);
}

void PrintTSFPGA::readMPUFIFO(MPU& mpu) {
    FPGAClass::readMPUFIFO(mpu);
    _printBufferU16("M<", mpu.getBuffer(), mpu.getLength());
}

void PrintTSFPGA::writeCommandFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    _printBufferU16("C>", data, length);
    FPGAClass::writeCommandFIFO(data, length, timeout);
}

void PrintTSFPGA::writeRequestFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    _printBufferU16("R>", data, length);
    FPGAClass::writeRequestFIFO(data, length, timeout);
}

void PrintTSFPGA::readU8ResponseFIFO(uint8_t* data, size_t length, uint32_t timeout) {
    FPGAClass::readU8ResponseFIFO(data, length, timeout);
    _printBufferU8("R8<", data, length);
}

void PrintTSFPGA::readU16ResponseFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    FPGAClass::readU16ResponseFIFO(data, length, timeout);
    _printBufferU16("R16<", data, length);
}

int main(int argc, char* const argv[]) { return cli.run(argc, argv); }
