/*
 * Abstract FPGA interface.
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

#include <string.h>
#include <thread>

#include <cRIO/ThermalILC.h>

#include "IFPGA.h"
#ifdef SIMULATOR
#include "SimulatedFPGA.h"
#else
#include "ThermalFPGA.h"
#endif

#include "Events/FcuTargets.h"
#include "Events/SummaryState.h"
#include "Settings/GlycolPump.h"
#include "Telemetry/FinerControl.h"

using namespace std::chrono_literals;
using namespace LSST::M1M3::TS;

IFPGA::IFPGA() : cRIO::FPGA(cRIO::fpgaType::TS) {
    _next_egw_powerup = std::chrono::steady_clock::now() +
                        std::chrono::seconds(Settings::GlycolPump::instance().communicationRecoverPowerOff);
}

IFPGA &IFPGA::get() {
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
    readU8ResponseFIFO(reinterpret_cast<uint8_t *>(&ret), 4, 10);
    return ret;
}

void IFPGA::setFCUPower(bool on) {
    uint16_t buf[2];
    buf[0] = FPGAAddress::FCU_ON;
    buf[1] = on;
    writeCommandFIFO(buf, 2, 10);
}

void IFPGA::setCoolantPumpPower(bool on) {
    if (on) {
        if (_next_egw_powerup > std::chrono::steady_clock::now()) {
            SPDLOG_INFO("Waiting for EGW pump power down.");
            std::this_thread::sleep_until(_next_egw_powerup);
        }
    } else {
        _next_egw_powerup =
                std::chrono::steady_clock::now() +
                std::chrono::seconds(Settings::GlycolPump::instance().communicationRecoverPowerOff);
    }
    uint16_t buf[2];
    buf[0] = FPGAAddress::COOLANT_PUMP_ON;
    buf[1] = on;
    writeCommandFIFO(buf, 2, 10);
}

void IFPGA::setHeartbeat(bool heartbeat) {
    uint16_t buf[2];
    buf[0] = FPGAAddress::HEARTBEAT;
    buf[1] = heartbeat;
    writeCommandFIFO(buf, 2, 0);
}

void IFPGA::panic() {
    Telemetry::FinerControl::instance().set_target(NAN);
    setMixingValvePosition(0);

    std::vector<int> zeros(0, cRIO::NUM_TS_ILC);
    try {
        Events::FcuTargets::instance().set_FCU_heaters_fans(zeros, zeros);
    } catch (std::exception &ex) {
        SPDLOG_WARN("Cannot zeroe fans and heaters on panic: {}.", ex.what());
    }

    setFCUPower(0);
}
