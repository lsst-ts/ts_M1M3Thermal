/*
 * SAL Thermal ILC.
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

#ifndef _TS_SALThermalILC_
#define _TS_SALThermalILC_

#include <cRIO/ThermalILC.h>
#include <SAL_MTM1M3TS.h>

#include <memory>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Stores data retrieved from ILCs into SAL structures.
 */
class SALThermalILC : public cRIO::ThermalILC {
public:
    SALThermalILC(std::shared_ptr<SAL_MTM1M3TS> m1m3tsSAL);

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
    void processReHeaterGains(uint8_t address, float proportionalGain, float integralGain) override;

private:
    std::shared_ptr<SAL_MTM1M3TS> _m1m3tsSAL;

    uint8_t _address2ILCIndex(uint8_t address);
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif
