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

#include <Settings/Setpoint.h>

using namespace LSST::M1M3::TS::Settings;

Setpoint::Setpoint(token) {
    low = NAN;
    high = NAN;
}

void Setpoint::load(YAML::Node doc) {
    SPDLOG_TRACE("Loading mixing valve settigns");
    try {
        timestep = doc["Timestep"].as<float>();
        precision = doc["Precision"].as<float>();

        low = doc["Low"].as<float>();
        high = doc["High"].as<float>();
    } catch (YAML::Exception &ex) {
        throw std::runtime_error(fmt::format("Cannot load Setpoint settings: {}", ex.what()));
    }
}
