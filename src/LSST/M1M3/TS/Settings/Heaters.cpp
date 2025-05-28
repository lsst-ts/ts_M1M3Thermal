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

#include <spdlog/spdlog.h>

#include <PID/PIDParameters.h>

#include "Settings/Heaters.h"

using namespace LSST::M1M3::TS::Settings;

Heaters::Heaters(token) { memset(heaters_PID, 0, sizeof(heaters_PID)); }

Heaters::~Heaters() {
    for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
        delete heaters_PID[i];
        heaters_PID[i] = nullptr;
    }
}

void Heaters::load(YAML::Node doc) {
    SPDLOG_INFO("Loading heaters settigns");
    auto pids = doc["PID"];

    PID::PIDParameters default_params;
    default_params.load(pids["Default"]);

    std::map<int, PID::PIDParameters> fcu_pid;

    for (auto fcu_specific : pids) {
        if (fcu_specific.first.as<std::string>() != "FCU") {
            continue;
        }
        PID::PIDParameters fcu_params;
        fcu_params.load(fcu_specific.second, default_params);
        fcu_pid.emplace(fcu_specific.second["Address"].as<int>() - 1, PID::PIDParameters(fcu_params));
    }

    for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
        delete heaters_PID[i];
        try {
            heaters_PID[i] = new PID::PID(fcu_pid.at(i));
            SPDLOG_DEBUG("FCU heaters custom PID {} - timestep: {} P: {} I: {} D: {} N: {}", i + 1,
                         fcu_pid[i].timestep, fcu_pid[i].P, fcu_pid[i].I, fcu_pid[i].D, fcu_pid[i].N);
        } catch (std::out_of_range &ex) {
            heaters_PID[i] = new PID::PID(default_params);
        }
    }
    interval = doc["Interval"].as<float>();
    if (interval <= 0) {
        throw std::runtime_error("Heaters/Interval must be greater than 0");
    }
}
