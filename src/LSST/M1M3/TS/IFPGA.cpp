/*
 * Abstract FPGA interface.
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

#include "IFPGA.h"

#include <string.h>

#ifdef SIMULATOR
#include <SimulatedFPGA.h>
#else
#include <ThermalFPGA.h>
#endif

using namespace LSST::M1M3::TS;

IFPGA::IFPGA() : cRIO::FPGA(cRIO::fpgaType::TS) {
    vfd = std::make_shared<VFD>(1, 100);
    flowMeter = std::make_shared<FlowMeter>(2, 1);
}

IFPGA& IFPGA::get() {
#ifdef SIMULATOR
    static SimulatedFPGA simulatedfpga;
    return simulatedfpga;
#else
    static ThermalFPGA thermalfpga;
    return thermalfpga;
#endif
}

float IFPGA::getMixingValvePosition() {
    uint16_t buf = FPGAAddress::MIXING_VALVE_POSITION;
    writeRequestFIFO(&buf, 1, 1);
    float ret;
    readSGLResponseFIFO(&ret, 1, 750);
    return ret;
}

void IFPGA::setMixingValvePosition(float position) {
    uint16_t buf[3];
    buf[0] = FPGAAddress::MIXING_VALVE_COMMAND;
    memcpy(buf + 1, &position, sizeof(float));
    writeCommandFIFO(buf, 3, 0);
}

uint32_t IFPGA::getSlot4DIs() {
    uint16_t buf = FPGAAddress::SLOT4_DIS;
    writeRequestFIFO(&buf, 1, 1);
    uint32_t ret;
    readU8ResponseFIFO(reinterpret_cast<uint8_t*>(&ret), 4, 10);
    return ret;
}

void IFPGA::setFCUPower(bool on) {
    uint16_t buf[2];
    buf[0] = FPGAAddress::FCU_ON;
    buf[1] = on;
    writeCommandFIFO(buf, 2, 10);
}

void IFPGA::setCoolantPumpPower(bool on) {
    uint16_t buf[2];
    buf[0] = FPGAAddress::COOLANT_PUMP_ON;
    buf[1] = on;
    writeCommandFIFO(buf, 2, 10);
}

void IFPGA::coolantPumpStartStop(bool start) {
    vfd->clearCommanded();
    vfd->presetHoldingRegister(0x2000, start ? 0x1a : 0x01);
    mpuCommands(*vfd);
}

void IFPGA::coolantPumpReset() {
    vfd->clearCommanded();
    vfd->presetHoldingRegister(0x2000, 0x08);
    mpuCommands(*vfd);
}

void IFPGA::setCoolantPumpFrequency(float freq) {
    vfd->clearCommanded();
    vfd->presetHoldingRegister(0x2001, freq * 10);
    mpuCommands(*vfd);
}

void IFPGA::setHeartbeat(bool heartbeat) {
    uint16_t buf[2];
    buf[0] = FPGAAddress::HEARTBEAT;
    buf[1] = heartbeat;
    writeCommandFIFO(buf, 2, 0);
}
