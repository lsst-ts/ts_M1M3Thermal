/*
 * EnabledILCs event.
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

#include <algorithm>

#include <cRIO/ThermalILC.h>
#include <ILC/ILCBusList.h>

#include "Events/ThermalWarning.h"
#include "Settings/FCUApplicationSettings.h"

using namespace LSST::M1M3::TS::Events;

ThermalWarning::ThermalWarning(token) {
    anyMajorFault = false;
    anyMinorFault = false;
    anyFaultOverride = false;
    anyRefResistorError = false;
    anyRTDError = false;
    anyBreakerHeater1Error = false;
    anyBreakerFan2Error = false;

    anyUniqueIdCRCError = false;
    anyApplicationTypeMismatch = false;
    anyApplicationCRCMismatch = false;
    anyOneWireMissing = false;
    anyOneWire1Mismatch = false;
    anyOneWire2Mismatch = false;
    anyWatchdogReset = false;
    anyBrownOut = false;
    anyEventTrapReset = false;
    anySSRPowerFault = false;
    anyAuxPowerFault = false;
}

void ThermalWarning::update(uint8_t _address, uint8_t mode, uint16_t status, uint16_t faults) {
    int index = _address - 1;
    auto update_field = [index, this](std::vector<bool> &values, bool new_value) {
        if (values[index] != new_value) {
            values[index] = new_value;
            _updated = true;
        }
    };
    update_field(majorFault, status & ILC::Status::MajorFault);
    update_field(minorFault, status & ILC::Status::MinorFault);
    update_field(faultOverride, status & ILC::Status::FaultOverride);
    update_field(refResistorError, status & cRIO::ThermalILCStatus::RefResistor);
    update_field(rtdError, status & cRIO::ThermalILCStatus::RTDError);
    update_field(breakerHeater1Error, status & cRIO::ThermalILCStatus::HeaterBreaker);
    update_field(breakerFan2Error, status & cRIO::ThermalILCStatus::FanBreaker);

    update_field(uniqueIdCRCError, faults & ILC::Fault::UniqueIdCRC);
    update_field(applicationTypeMismatch, faults & ILC::Fault::AppType);
    update_field(applicationCRCMismatch, faults & ILC::Fault::AppCRC);
    update_field(oneWireMissing, faults & ILC::Fault::NoTEDS);
    update_field(oneWire1Mismatch, faults & ILC::Fault::TEDS1);
    update_field(oneWire2Mismatch, faults & ILC::Fault::TEDS2);
    update_field(watchdogReset, faults & ILC::Fault::WatchdogReset);
    update_field(brownOut, faults & ILC::Fault::BrownOut);
    update_field(eventTrapReset, faults & ILC::Fault::EventTrap);
    update_field(ssrPowerFault, faults & ILC::Fault::SSR);
    update_field(auxPowerFault, faults & ILC::Fault::AUX);
}

void ThermalWarning::send() {
    if (_updated) {
        auto check_any = [](const std::vector<bool> &values) -> bool {
            return std::find(values.begin(), values.end(), true) != values.end();
        };

        anyMajorFault = check_any(majorFault);
        anyMinorFault = check_any(minorFault);
        anyFaultOverride = check_any(faultOverride);
        anyRefResistorError = check_any(refResistorError);
        anyRTDError = check_any(rtdError);
        anyBreakerHeater1Error = check_any(breakerHeater1Error);
        anyBreakerFan2Error = check_any(breakerFan2Error);

        anyUniqueIdCRCError = check_any(uniqueIdCRCError);
        anyApplicationTypeMismatch = check_any(applicationTypeMismatch);
        anyApplicationCRCMismatch = check_any(applicationCRCMismatch);
        anyOneWireMissing = check_any(oneWireMissing);
        anyOneWire1Mismatch = check_any(oneWire1Mismatch);
        anyOneWire2Mismatch = check_any(oneWire2Mismatch);
        anyWatchdogReset = check_any(watchdogReset);
        anyBrownOut = check_any(brownOut);
        anyEventTrapReset = check_any(eventTrapReset);
        anySSRPowerFault = check_any(ssrPowerFault);
        anyAuxPowerFault = check_any(auxPowerFault);

        TSPublisher::instance().logThermalWarning(this);
    }
}
