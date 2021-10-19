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

#include <cRIO/ThermalILC.h>
#include <Settings/Thermal.h>
#include <Events/EnabledILC.h>

#include <algorithm>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

using namespace LSST::M1M3::TS::Settings;

void Thermal::load(const std::string &filename) {
    SPDLOG_DEBUG("Thermal::load(\"{}\")", filename);

    try {
        YAML::Node doc = YAML::LoadFile(filename);

        auto disabledIndices = doc["DisabledFCU"].as<std::vector<int>>();

        for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
            enabledFCU[i] =
                    std::find(disabledIndices.begin(), disabledIndices.end(), i) == disabledIndices.end();
            Events::EnabledILC::instance().setEnabled(i, enabledFCU[i]);
        }
    } catch (YAML::Exception &ex) {
        throw std::runtime_error(fmt::format("YAML Loading {}: {}", filename, ex.what()));
    }

    log();
    Events::EnabledILC::instance().send();
}
