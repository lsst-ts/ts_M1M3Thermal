/*
 * This file is part of LSST M1M3 thermal system package.
 *
 * Developed for the LSST Telescope & Site Software
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
#include <yaml-cpp/yaml.h>

#include "Settings/AirNozzles.h"
#include "Settings/Controller.h"
#include "Settings/FlowMeter.h"
#include "Settings/GlycolPump.h"
#include "Settings/Heaters.h"
#include "Settings/MixingValve.h"
#include "Settings/Setpoint.h"
#include "Settings/Thermal.h"

using namespace LSST::M1M3::TS::Settings;

void Controller::load(const std::string &configuration_override) {
    std::string filename = cRIO::Settings::Path::getFilePath("v1/_init.yaml");
    SPDLOG_INFO("Using configuration file \"{}\"", filename);
    try {
        YAML::Node doc = YAML::LoadFile(filename);

        FlowMeter::instance().load(doc["FlowMeter"]);
        GlycolPump::instance().load(doc["GlycolPump"]);
        MixingValve::instance().load(doc["MixingValve"]);
        Heaters::instance().load(doc["Heaters"]);
        Setpoint::instance().load(doc["Setpoint"]);
        Thermal::instance().load(doc["FCU"]);
        AirNozzles::instance().load("AirNozzles.csv");
    } catch (YAML::Exception &ex) {
        auto msg = fmt::format("YAML Loading {}:{}:{} (line, column): {}", filename, ex.mark.line,
                               ex.mark.column, ex.what());
        SPDLOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
}
