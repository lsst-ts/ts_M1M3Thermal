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

#include "Events/AppliedSetpoints.h"
#include "Settings/SavedSetpoints.h"
#include "Settings/Setpoint.h"

using namespace LSST::M1M3::TS::Settings;

Setpoint::Setpoint(token) : _saved_setpoints(nullptr) {
    glycolSupplyPercentage = 100;
    safetyAirTemperatureMaxAge = 600;

    low = NAN;
    high = NAN;
}

Setpoint::~Setpoint() { delete _saved_setpoints; }

void Setpoint::load(YAML::Node doc) {
    SPDLOG_INFO("Loading Setpoint settings.");
    timestep = doc["Timestep"].as<float>();
    mixingValveStep = doc["MixingValveStep"].as<float>();

    glycolSupplyPercentage = doc["GlycolSupplyPercentage"].as<float>();
    if (glycolSupplyPercentage < 0 || glycolSupplyPercentage > 100) {
        throw std::runtime_error(
                fmt::format("Setpoint/GlycolSupplyPercentage configuration parameter must be between 0 "
                            "and 100, was set to {:.2f}.",
                            glycolSupplyPercentage));
    }

    low = doc["Low"].as<float>();
    high = doc["High"].as<float>();
    if (low >= high) {
        throw std::runtime_error(fmt::format(
                "Setpoint/Low configuration parameter must be less than Setpoint/High: {} {}.", low, high));
    }

    auto safety = doc["Safety"];

    safetyRange = safety["Range"].as<float>();

    safetyMinOffset = safety["MinOffset"].as<float>();
    if (safetyMinOffset <= 0 || safetyMinOffset >= 2) {
        throw std::runtime_error(fmt::format(
                "Setpoint/Safety/MinOffset configuration parameter shall be between 0 and 2, is {:.02f}.",
                safetyMinOffset));
    }

    safetyHeatersOffset = safety["HeatersOffset"].as<float>();

    if (abs(safetyHeatersOffset) > 3) {
        throw std::runtime_error(
                fmt::format("Setpoint/Safety/HeatersOffset configuration parameter absolute value shall "
                            "be < 3, is {:.02f}.",
                            safetyHeatersOffset));
    }

    safetyMaxViolations = safety["MaxViolations"].as<int>();

    safetyAirTemperatureMaxAge = safety["AirTemperatureMaxAge"].as<float>();

    delete _saved_setpoints;

    auto save = doc["Save"];

    savedSetpointsMaxAge = save["MaxAge"].as<uint64_t>();
    _saved_setpoints = new SavedSetpoints(save["Filename"].as<std::string>());

    _saved_setpoints->load();
}

void Setpoint::save_setpoints(float glycol, float heaters) {
    if (_saved_setpoints == nullptr) {
        SPDLOG_ERROR("Save setpoints called without valid setpoint file!");
        return;
    }
    _saved_setpoints->save(glycol, heaters);
}

void Setpoint::apply_saved_setpoints() {
    Events::AppliedSetpoints::instance().set_applied_setpoints(_saved_setpoints->glycol(),
                                                               _saved_setpoints->heaters());
}
