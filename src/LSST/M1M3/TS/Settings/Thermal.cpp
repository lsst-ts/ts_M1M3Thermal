/*
 * This file is part of LSST M1M3 thermal system package.
 *
 * Developed for the LSST Data Management System.
 * This product includes software developed by the LSST Project
 * (https://www.lsst.org).
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <spdlog/spdlog.h>

#include <cRIO/ThermalILC.h>

#include <Events/EnabledILC.h>
#include <Settings/Thermal.h>

using namespace LSST::M1M3::TS::Settings;

void Thermal::load(YAML::Node doc) {
    SPDLOG_INFO("Loading disabled FCU list");

    autoDisable = doc["AutoDisable"].as<bool>();
    failuresToDisable = doc["FailuresToDisable"].as<int>();

    auto disabledIndices = doc["Disabled"].as<std::vector<int>>();

    for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
        enabledFCU[i] =
                std::find(disabledIndices.begin(), disabledIndices.end(), i + 1) == disabledIndices.end();
        Events::EnabledILC::instance().setEnabled(i, enabledFCU[i]);
    }

    defaultFanSpeed = doc["DefaultFanSpeed"].as<int>();

    log();
    Events::EnabledILC::instance().send();
}
