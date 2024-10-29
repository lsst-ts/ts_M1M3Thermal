/*
 * Command line Thermal System client.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software
 * Systems. This product includes software developed by the Vera C.Rubin
 * Observatory Project (https://www.lsst.org). See the COPYRIGHT file at the
 * top-level directory of this distribution for details of code ownership.
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

#include <iomanip>
#include <iostream>
#include <memory>

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cRIO/FPGACliApp.h>
#include <cRIO/MPU.h>
#include <cRIO/PrintILC.h>
#include <cRIO/ThermalILC.h>

#ifndef SIMULATOR
#include <Transports/FPGASerialDevice.h>
#endif

#ifdef SIMULATOR
#include <SimulatedFPGA.h>
#define FPGAClass SimulatedFPGA
#else
#include <NiFpga/NiFpga_ts_M1M3ThermalFPGA.h>
#include <ThermalFPGA.h>
#define FPGAClass ThermalFPGA
#endif

#include <MPU/FactoryInterface.h>
#include <MPU/FlowMeter.h>
#include <MPU/GlycolTemperature.h>
#ifdef SIMULATOR
#include <MPU/SimulatedFlowMeter.h>
#include <MPU/SimulatedVFDPump.h>
#include <MPU/SimulatedGlycolTemperature.h>
#endif
#include <MPU/VFD.h>

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;
using namespace std::chrono_literals;

#ifdef SIMULATOR
class PrintFlowMeterDevice : public SimulatedFlowMeter {
    void write(const unsigned char *buf, size_t len) override;
    std::vector<uint8_t> read(size_t len, std::chrono::microseconds timeout,
                              LSST::cRIO::Thread *calling_thread = NULL) override;
};

class PrintVFDPumpDevice : public SimulatedVFDPump {
    void write(const unsigned char *buf, size_t len) override;
    std::vector<uint8_t> read(size_t len, std::chrono::microseconds timeout,
                              LSST::cRIO::Thread *calling_thread = NULL) override;
};

#else
class PrintFPGASerialDevice : public Transports::FPGASerialDevice {
public:
    PrintFPGASerialDevice(uint32_t fpga_session, int write_fifo, int read_fifo,
                          std::chrono::microseconds quiet_time)
            : Transports::FPGASerialDevice(fpga_session, write_fifo, read_fifo, quiet_time) {}
    void write(const unsigned char *buf, size_t len) override;
    std::vector<uint8_t> read(size_t len, std::chrono::microseconds timeout,
                              LSST::cRIO::Thread *calling_thread = NULL) override;
};
#endif

class M1M3TScli : public FPGACliApp {
public:
    M1M3TScli(const char *name, const char *description);

    int openFPGA(command_vec cmds) override;

    int mpuRead(command_vec cmds);
    int mpuFlush(command_vec cmds);
    int mpuTelemetry(command_vec cmds);
    int mpuWrite(command_vec cmds);
    int printFlowMeter(command_vec cmds);
    int printPump(command_vec cmds);
    int mixingValve(command_vec cmds);
    int fcuOnOff(command_vec cmds);
    int pumpOnOff(command_vec cmds);
    int fcuBroadcast(command_vec cmds);
    int fcuDemand(command_vec cmds);
    int setReHeaterGain(command_vec cmds);
    int chassisTemperature(command_vec cmds);
    int glycolTemperature(command_vec cmds);
    int slot4(command_vec);
    int ilcPower(command_vec);

protected:
    virtual FPGA *newFPGA(const char *dir, bool &fpga_singleton) override;
    virtual ILCUnits getILCs(command_vec cmds) override;

private:
    void printTelemetry(const std::string &name, std::shared_ptr<MPU> mpu);

    std::shared_ptr<Transports::Transport> get_transport(std::shared_ptr<MPU> mpu);

    std::shared_ptr<FlowMeterPrint> flowMeter;
    std::shared_ptr<VFDPrint> vfd;
    std::shared_ptr<GlycolTemperature> glycolTemperatureBus;

    std::shared_ptr<Transports::Transport> _flow_meter_device, _vfd_device, _glycol_temperature_device;
};

class PrintThermalILC : public ThermalILC, public PrintILC {
public:
    PrintThermalILC(uint8_t bus) : ILCBusList(bus), ThermalILC(bus), PrintILC(bus) {}

protected:
    void processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature, uint8_t fanRPM,
                              float absoluteTemperature) override;
    void processReHeaterGains(uint8_t address, float proportionalGain, float integralGain) override;
};

class PrintTSFPGA : public FPGAClass {
public:
#ifdef SIMULATOR
    PrintTSFPGA() : ILCBusList(1), FPGAClass() {}
#else
    PrintTSFPGA() : FPGAClass() {}
#endif
    void writeCommandFIFO(uint16_t *data, size_t length, uint32_t timeout) override;
    void writeRequestFIFO(uint16_t *data, size_t length, uint32_t timeout) override;
    void readU8ResponseFIFO(uint8_t *data, size_t length, uint32_t timeout) override;
    void readU16ResponseFIFO(uint16_t *data, size_t length, uint32_t timeout) override;

private:
    void _printTimestamp(std::string prefix, bool nullTimer);
};

class PrintMPUFactory : public FactoryInterface {
public:
    PrintMPUFactory(std::shared_ptr<FlowMeterPrint> flowMeter, std::shared_ptr<VFDPrint> vfd) {
        _flowMeter = flowMeter;
        _vfd = vfd;
    }

    std::shared_ptr<FlowMeter> createFlowMeter() override { return _flowMeter; }
    std::shared_ptr<VFD> createVFD() override { return _vfd; }

private:
    std::shared_ptr<FlowMeterPrint> _flowMeter;
    std::shared_ptr<VFDPrint> _vfd;
};

#define ILC_ARG "<ILC..>"

M1M3TScli::M1M3TScli(const char *name, const char *description) : FPGACliApp(name, description) {
    addCommand("mpu-read", std::bind(&M1M3TScli::mpuRead, this, std::placeholders::_1), "SS?", NEED_FPGA,
               "<mpu> <register[:length]>..", "Reads given MPU registers");
    addCommand("mpu-flush", std::bind(&M1M3TScli::mpuFlush, this, std::placeholders::_1), "s", NEED_FPGA,
               "[mpu]", "Flush MPU port");
    addCommand("mpu-telemetry", std::bind(&M1M3TScli::mpuTelemetry, this, std::placeholders::_1), "s",
               NEED_FPGA, "[mpu]", "Reads MPU telemetry");
    addCommand("mpu-write", std::bind(&M1M3TScli::mpuWrite, this, std::placeholders::_1), "SII", NEED_FPGA,
               "<mpu> <register> <value>", "Writes give MPU registers");
    addCommand("flow", std::bind(&M1M3TScli::printFlowMeter, this, std::placeholders::_1), "", NEED_FPGA,
               NULL, "Reads FlowMeter values");
    addCommand("mixing-valve", std::bind(&M1M3TScli::mixingValve, this, std::placeholders::_1), "d",
               NEED_FPGA, "[valve position (mA)]", "Reads and sets mixing valve position");
    addCommand("fcu-on", std::bind(&M1M3TScli::fcuOnOff, this, std::placeholders::_1), "b", NEED_FPGA,
               "[on|off]", "Command FCU power on/off");
    addCommand("pump", std::bind(&M1M3TScli::printPump, this, std::placeholders::_1), "s?", NEED_FPGA, NULL,
               "Turns pump on and reads Pump VFD values");

    addCommand("fcu-broadcast", std::bind(&M1M3TScli::fcuBroadcast, this, std::placeholders::_1), "ii",
               NEED_FPGA, "<heater PWM> <fan RPM>",
               "Broadcast ILC heater and fan demand, set all ILC to the same value");
    addCommand("fcu-demand", std::bind(&M1M3TScli::fcuDemand, this, std::placeholders::_1), "iis?", NEED_FPGA,
               "<heater PWM> <fan RPM> " ILC_ARG, "Sets FCU heater and fan");
    addCommand("slot4", std::bind(&M1M3TScli::slot4, this, std::placeholders::_1), "", NEED_FPGA, NULL,
               "Reads slot 4 inputs");
    addCommand("ilc-power", std::bind(&M1M3TScli::ilcPower, this, std::placeholders::_1), "B", NEED_FPGA,
               "<on/off>", "Sets ILC power at L5 (CH3 of Mod 5)");

    addILCCommand(
            "fcu-status",
            [](ILCUnit u) {
                std::dynamic_pointer_cast<PrintThermalILC>(u.first)->reportThermalStatus(u.second);
            },
            "Report ILC Thermal Status");

    addILCCommand(
            "reheater-gains",
            [](ILCUnit u) {
                std::dynamic_pointer_cast<PrintThermalILC>(u.first)->reportReHeaterGains(u.second);
            },
            "Report ILC Re-Heater Gains");
    addCommand("set-reheater-gains", std::bind(&M1M3TScli::setReHeaterGain, this, std::placeholders::_1),
               "DDS?", NEED_FPGA, "<proportionalGain> <integralGain> " ILC_ARG, "Set ILC Re-Heater Gain");

    addCommand("chassis-temperature", std::bind(&M1M3TScli::chassisTemperature, this, std::placeholders::_1),
               "", NEED_FPGA, NULL, "Reports chassis temperature");

    addCommand("glycol-temperature", std::bind(&M1M3TScli::glycolTemperature, this, std::placeholders::_1),
               "", NEED_FPGA, NULL, "Primts glycol temperature values");

    addILC(std::make_shared<PrintThermalILC>(1));

    flowMeter = std::make_shared<FlowMeterPrint>();
    addMPU("flow", flowMeter);

    vfd = std::make_shared<VFDPrint>();
    addMPU("vfd", vfd);

#ifdef SIMULATOR
    std::cout << "Starting SIMULATED m1m3tscli!" << std::endl;
#endif
}

int M1M3TScli::openFPGA(command_vec cmds) {
    int ret = FPGACliApp::openFPGA(cmds);

#ifdef SIMULATOR
    _flow_meter_device = std::make_shared<SimulatedFlowMeter>();
    _vfd_device = std::make_shared<SimulatedVFDPump>();
    _glycol_temperature_device = std::make_shared<SimulatedGlycolTemperature>();
#else
    int session = dynamic_cast<ThermalFPGA *>(getFPGA())->getSession();

    _flow_meter_device = std::make_shared<PrintFPGASerialDevice>(
            session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_FlowMeterWrite,
            NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_FlowMeterRead, 10ms);

    _vfd_device = std::make_shared<PrintFPGASerialDevice>(
            session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_GlycoolWrite,
            NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_GlycoolRead, 10ms);

    _glycol_temperature_device = std::make_shared<Transports::FPGASerialDevice>(
            session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_CoolantTempWrite,
            NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_CoolantTempRead, 1ms);
#endif

    glycolTemperatureBus = std::make_shared<GlycolTemperature>(_glycol_temperature_device);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    addThread(glycolTemperatureBus.get());

    return ret;
}

int M1M3TScli::mpuRead(command_vec cmds) {
    auto mpu = getMPU(cmds[0]);
    auto transport = get_transport(mpu);
    if (mpu == NULL || transport == NULL) {
        std::cerr << "Invalid MPU device name " << cmds[0] << ". List of known devices: " << std::endl;
        printMPU();
        return -1;
    }

    mpu->reset();

    std::vector<std::pair<uint16_t, uint8_t>> registers;

    for (size_t i = 1; i < cmds.size(); i++) {
        auto sep = cmds[i].find(':');
        if (sep != std::string::npos) {
            registers.push_back(std::pair<uint16_t, uint8_t>(stoi(cmds[i], nullptr, 0),
                                                             stoi(cmds[i].substr(sep + 1), nullptr, 0)));
        } else {
            registers.push_back(std::pair<uint16_t, uint8_t>(stoi(cmds[i], nullptr, 0), 1));
        }
    }

    for (auto r : registers) {
        mpu->readHoldingRegisters(r.first, r.second, 255);
        mpu->clear();
    }

    transport->commands(*mpu, 2s);

    for (auto r : registers) {
        for (int i = 0; i < r.second; i++) {
            uint16_t v = mpu->getRegister(r.first + i);
            std::cout << fmt::format("{0:>5d} (0x{0:04x}): {1:d} (0x{1:x})", r.first + i, v) << std::endl;
        }
    }

    return 0;
}

int M1M3TScli::mpuFlush(command_vec cmds) {
    auto mpu = getMPU(cmds[0]);
    auto transport = get_transport(mpu);
    if (mpu == NULL || transport == NULL) {
        std::cerr << "Invalid MPU device name " << cmds[0] << ". List of known devices: " << std::endl;
        printMPU();
        return -1;
    }

    transport->flush();

    return 0;
}

int M1M3TScli::mpuTelemetry(command_vec cmds) {
    if (cmds.size() == 0) {
        printTelemetry("Flow meter", getMPU("flow"));
        printTelemetry("Pump (VFD)", getMPU("vfd"));
        printTelemetry("Glycol temperature", glycolTemperatureBus);
    } else {
        printTelemetry(cmds[0], getMPU(cmds[0]));
    }

    return 0;
}

int M1M3TScli::mpuWrite(command_vec cmds) {
    std::shared_ptr<MPU> mpu = getMPU(cmds[0]);
    if (mpu == NULL) {
        std::cerr << "Invalid MPU device name " << cmds[0] << ". List of known devices: " << std::endl;
        printMPU();
        return -1;
    }
    auto transport = get_transport(mpu);

    mpu->clear();
    mpu->reset();

    uint16_t addrs = stoi(cmds[1], nullptr, 0);
    uint16_t value = stoi(cmds[2], nullptr, 0);

    mpu->presetHoldingRegister(addrs, value);

    mpu->reset();
    mpu->clear();

    transport->commands(*mpu, 2s);

    return 0;
}

int M1M3TScli::printFlowMeter(command_vec cmds) {
    flowMeter->readInfo();

    _flow_meter_device->commands(*flowMeter, 2s);

    flowMeter->print();

    return 0;
}

int M1M3TScli::printPump(command_vec cmds) {
    vfd->clear();

    IFPGA *fpga = dynamic_cast<IFPGA *>(getFPGA());
    if (cmds.size() > 0) {
        if (cmds[0] == "stop") {
            vfd->stop();
        } else if (cmds[0] == "start") {
            vfd->start();
        } else if (cmds[0] == "reset") {
            vfd->reset();
        } else if (cmds[0] == "freq") {
            size_t len;
            uint16_t targetFreq = std::stod(cmds[1], &len);
            if (len != cmds[1].length()) {
                std::cerr << "Invalid frequency: " << cmds[1] << std::endl;
                return 1;
            }
            vfd->setFrequency(targetFreq);
        } else {
            fpga->setCoolantPumpPower(onOff(cmds[0]));
            std::cout << "Turned pump " << cmds[0] << std::endl;
            return 0;
        }

        _vfd_device->commands(*vfd, 2s);
    }

    vfd->readInfo();

    _vfd_device->commands(*vfd, 2s);

    vfd->print();

    return 0;
}

int M1M3TScli::mixingValve(command_vec cmds) {
    if (cmds.size() == 1) {
        dynamic_cast<IFPGA *>(getFPGA())->setMixingValvePosition(std::stof(cmds[0]) / 1000.0f);
    }
    std::cout << "Mixing valve: " << std::fixed << std::setprecision(3)
              << dynamic_cast<IFPGA *>(getFPGA())->getMixingValvePosition() << " V" << std::endl;
    return 0;
}

int M1M3TScli::fcuOnOff(command_vec cmds) {
    if (cmds.size() == 1) {
        dynamic_cast<IFPGA *>(getFPGA())->setFCUPower(onOff(cmds[0]));
        std::cout << "Turned FCU power " << cmds[0] << std::endl;
    }
    return 0;
}

int M1M3TScli::pumpOnOff(command_vec cmds) { return 0; }

FPGA *M1M3TScli::newFPGA(const char *dir, bool &fpga_singleton) {
    PrintTSFPGA *printFPGA = new PrintTSFPGA();
    printFPGA->setMPUFactory(std::make_shared<PrintMPUFactory>(flowMeter, vfd));
    return printFPGA;
}

int M1M3TScli::fcuBroadcast(command_vec cmds) {
    uint8_t heater = std::stoi(cmds[0]);
    uint8_t fan = std::stoi(cmds[1]);

    uint8_t heater_data[NUM_TS_ILC];
    uint8_t fan_data[NUM_TS_ILC];

    memset(heater_data, heater, NUM_TS_ILC);
    memset(fan_data, fan, NUM_TS_ILC);

    std::dynamic_pointer_cast<PrintThermalILC>(getILC(0))->broadcastThermalDemand(heater_data, fan_data);
    getFPGA()->ilcCommands(*getILC(0), ilcTimeout);
    return 0;
}

int M1M3TScli::fcuDemand(command_vec cmds) {
    uint8_t heater = std::stoi(cmds[0]);
    uint8_t fan = std::stoi(cmds[1]);
    cmds.erase(cmds.begin(), cmds.begin() + 2);

    clearILCs();
    ILCUnits ilcs = getILCs(cmds);
    for (auto u : ilcs) {
        std::dynamic_pointer_cast<PrintThermalILC>(u.first)->setThermalDemand(u.second, heater, fan);
    }
    getFPGA()->ilcCommands(*getILC(0), ilcTimeout);
    return 0;
}

int M1M3TScli::setReHeaterGain(command_vec cmds) {
    float proportionalGain = std::stof(cmds[0]);
    float integralGain = std::stof(cmds[1]);
    cmds.erase(cmds.begin(), cmds.begin() + 2);

    clearILCs();
    ILCUnits ilcs = getILCs(cmds);
    for (auto u : ilcs) {
        std::dynamic_pointer_cast<PrintThermalILC>(u.first)->setReHeaterGains(u.second, proportionalGain,
                                                                              integralGain);
    }
    getFPGA()->ilcCommands(*getILC(0), ilcTimeout);
    return 0;
}

int M1M3TScli::chassisTemperature(command_vec cmds) {
    float temperature = dynamic_cast<IFPGA *>(getFPGA())->chassisTemperature();
    std::cout << "Chassis temperature: " << std::fixed << std::setprecision(2) << temperature << std::endl;

    return 0;
}

int M1M3TScli::glycolTemperature(command_vec) {
    if (getDebugLevel() > 0) {
        std::cout << "Buffer: " << glycolTemperatureBus->getDataBuffer() << std::endl << std::endl;
    }
    auto temp = glycolTemperatureBus->getTemperatures();
    const char *names[8] = {"Above Mirror",          "Inside Cell 1",        "Inside Cell 2",
                            "Inside Cell 3",         "MTA Coolant Supply",   "MTA Coolant Return",
                            "Mirror Coolant Supply", "Mirror Coolant Return"};
    for (int i = 0; i < 8; i++) {
        std::cout << std::right << std::setw(30) << names[i] << ": " << std::setw(6) << std::fixed
                  << std::setprecision(4) << temp[i] << std::endl;
    }
    return 0;
}

int M1M3TScli::slot4(command_vec) {
    uint32_t dis = dynamic_cast<IFPGA *>(getFPGA())->getSlot4DIs();
    std::cout << "Slot4: 0x" << std::hex << std::setfill('0') << std::setw(4) << dis << std::endl
              << std::setfill(' ') << std::endl;

    const char *names[32] = {
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

int M1M3TScli::ilcPower(command_vec cmds) {
    uint16_t buf[2] = {FPGAAddress::ILC_POWER, onOff(cmds[0])};
    dynamic_cast<IFPGA *>(getFPGA())->writeCommandFIFO(buf, 2, 10);
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
                if ((address <= 0 || address > NUM_TS_ILC) && (address != 255)) {
                    std::cerr << "Invalid address " << c << std::endl;
                    ret = -1;
                    continue;
                }
                units.push_back(ILCUnit(getILC(0), address));
            } catch (std::logic_error &e) {
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

void M1M3TScli::printTelemetry(const std::string &name, std::shared_ptr<MPU> mpu) {
    uint64_t send;
    uint64_t received;
    auto transport = get_transport(mpu);
    transport->telemetry(send, received);
    std::cout << name << std::endl
              << std::string(name.length(), '_') << std::endl
              << "Send: " << send << std::endl
              << "Received: " << received << std::endl;
}

std::shared_ptr<Transports::Transport> M1M3TScli::get_transport(std::shared_ptr<MPU> mpu) {
    if (mpu == vfd) {
        return _vfd_device;
    } else if (mpu == flowMeter) {
        return _flow_meter_device;
    } else if (mpu == glycolTemperatureBus) {
        return _glycol_temperature_device;
    }
    throw std::runtime_error("Unknow serial device");
}

void PrintThermalILC::processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature,
                                           uint8_t fanRPM, float absoluteTemperature) {
    printBusAddress(address);
    std::cout << "Thermal ILC Status: 0x" << std::hex << std::setfill('0') << std::setw(4) << +status << ": "
              << fmt::format("{}", fmt::join(getThermalStatusString(status), " | ")) << std::endl
              << "Differential Temperature: " << std::to_string(differentialTemperature) << std::endl
              << "Fan RPM: " << std::to_string(fanRPM) << std::endl
              << "Absolute Temperature: " << std::to_string(absoluteTemperature) << std::endl;
}

void PrintThermalILC::processReHeaterGains(uint8_t address, float proportionalGain, float integralGain) {
    printBusAddress(address);
    std::cout << "Re-Heater Proportional Gain: " << std::to_string(proportionalGain) << std::endl
              << "Re-Heater Integral Gain: " << std::to_string(integralGain) << std::endl;
}

std::chrono::time_point<std::chrono::steady_clock> _cmd_start;

M1M3TScli cli("M1M3TS", "M1M3 Thermal System Command Line Interface");

void _printTimestamp(std::string prefix, bool nullTimer) {
    if (cli.getDebugLevel() > 3) {
        if (nullTimer) {
            std::cout << "0.000.000 - ";
        } else {
            std::chrono::duration<double> diff = std::chrono::steady_clock::now() - _cmd_start;
            auto count = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
            auto sdv = std::ldiv(count, 1000000);
            int s = sdv.quot;
            sdv = std::ldiv(sdv.rem, 1000);
            std::cout << std::dec << std::setw(1) << std::setfill('0') << s << "." << std::setw(3) << sdv.quot
                      << "." << std::setw(3) << sdv.rem << " - ";
        }
    }

    _cmd_start = std::chrono::steady_clock::now();

    std::cout << prefix;
}

void _printBufferU8(std::string prefix, bool nullTimer, const uint8_t *buf, size_t len) {
    if (cli.getDebugLevel() == 0) {
        return;
    }

    _printTimestamp(prefix, nullTimer);

    CliApp::printHexBuffer(buf, len);

    std::cout << std::endl;
}

void _printBufferU8(std::string prefix, bool nullTimer, const std::vector<uint8_t> &buf) {
    _printBufferU8(prefix, nullTimer, buf.data(), buf.size());
}

void _printBufferU16(std::string prefix, bool nullTimer, uint16_t *buf, size_t len) {
    if (cli.getDebugLevel() == 0) {
        return;
    }

    _printTimestamp(prefix, nullTimer);

    CliApp::printHexBuffer(buf, len);

    if (cli.getDebugLevel() > 1 && len > 1) {
        std::cout << std::endl;
        if (cli.getDebugLevel() > 3) {
            std::cout << "|.|||.||| - ";
        }
        std::cout << prefix;
        CliApp::printDecodedBuffer(buf, len);
    }
    std::cout << std::endl;
}

#ifdef SIMULATOR
void PrintFlowMeterDevice::write(const unsigned char *buf, size_t len) {
    _printBufferU8("FlowMeter > ", true, buf, len);
    SimulatedFlowMeter::write(buf, len);
}

std::vector<uint8_t> PrintFlowMeterDevice::read(size_t len, std::chrono::microseconds timeout,
                                                LSST::cRIO::Thread *calling_thread) {
    auto ret = SimulatedFlowMeter::read(len, timeout, calling_thread);
    _printBufferU8("FlowMeter < ", true, ret);
    return ret;
}

void PrintVFDPumpDevice::write(const unsigned char *buf, size_t len) {
    _printBufferU8("VFD Pump > ", true, buf, len);
    SimulatedVFDPump::write(buf, len);
}

std::vector<uint8_t> PrintVFDPumpDevice::read(size_t len, std::chrono::microseconds timeout,
                                              LSST::cRIO::Thread *calling_thread) {
    auto ret = SimulatedVFDPump::read(len, timeout, calling_thread);
    _printBufferU8("VFD Pump < ", true, ret);
    return ret;
}
#else
void PrintFPGASerialDevice::write(const unsigned char *buf, size_t len) {
    if (len <= 0) {
        throw std::runtime_error("MPU - 0 buffer");
    }
    _printBufferU8("MPU > ", true, buf, len);
    FPGASerialDevice::write(buf, len);
}

std::vector<uint8_t> PrintFPGASerialDevice::read(size_t len, std::chrono::microseconds timeout,
                                                 LSST::cRIO::Thread *calling_thread) {
    auto ret = FPGASerialDevice::read(len, timeout, calling_thread);
    _printBufferU8("MPU < ", true, ret);
    return ret;
}
#endif

void PrintTSFPGA::writeCommandFIFO(uint16_t *data, size_t length, uint32_t timeout) {
    _printBufferU16("C>", true, data, length);
    FPGAClass::writeCommandFIFO(data, length, timeout);
}

void PrintTSFPGA::writeRequestFIFO(uint16_t *data, size_t length, uint32_t timeout) {
    _printBufferU16("R>", false, data, length);
    FPGAClass::writeRequestFIFO(data, length, timeout);
}

void PrintTSFPGA::readU8ResponseFIFO(uint8_t *data, size_t length, uint32_t timeout) {
    FPGAClass::readU8ResponseFIFO(data, length, timeout);
    _printBufferU8("R8<", false, data, length);
}

void PrintTSFPGA::readU16ResponseFIFO(uint16_t *data, size_t length, uint32_t timeout) {
    FPGAClass::readU16ResponseFIFO(data, length, timeout);
    _printBufferU16("R16<", false, data, length);
}

int main(int argc, char *const argv[]) { return cli.run(argc, argv); }
