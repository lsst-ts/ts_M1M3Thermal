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

#ifdef SIMULATOR

#include <spdlog/spdlog.h>

#include "Settings/Simulator.h"

using namespace LSST::M1M3::TS::Settings;

Simulator::Simulator(token) {}

void Simulator::load(YAML::Node doc) {
    wrong_application_type_address =
            doc["wrong_application_type_address"].as<int>(wrong_application_type_address);
}

#endif
