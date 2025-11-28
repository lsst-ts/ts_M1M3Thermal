/*
 * VFD EGW Pump Driver Status2
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

#include "Events/DriveStatus2.h"

using namespace LSST::M1M3::TS::Events;

DriveStatus2::DriveStatus2(token) { drive_status_2 = 0xFF; }

void DriveStatus2::set(uint16_t _drive_status) {
    if (_drive_status != drive_status_2) {
        jogging = drive_status_2 & STATUS2::JOGGING;
        fluxBreaking = drive_status_2 & STATUS2::FLUX_BREAKING;
        motorOverload = drive_status_2 & STATUS2::MOTOR_OVERLOAD;
        autoRestartCountdown = drive_status_2 & STATUS2::AUTORST_CTDN;
        dcBraking = drive_status_2 & STATUS2::DC_BRAKING;
        atFrequency = drive_status_2 & STATUS2::AT_FREQUENCY;
        autoTuning = drive_status_2 & STATUS2::AUTO_TUNING;
        emBraking = drive_status_2 & STATUS2::EM_BRAKING;
        currentLimit = drive_status_2 & STATUS2::CURRENT_LIMIT;
        safetyS1 = drive_status_2 & STATUS2::SAFETY_S1;
        safetyS2 = drive_status_2 & STATUS2::SAFETY_S2;
        f111Status = drive_status_2 & STATUS2::F111_STATUS;
        safeTqPermit = drive_status_2 & STATUS2::SAFETQPERMIT;

        drive_status_2 = _drive_status;

        send();
    }
}
