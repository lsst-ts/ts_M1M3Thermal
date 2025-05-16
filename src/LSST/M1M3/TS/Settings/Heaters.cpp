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

#include <Settings/Heaters.h>

using namespace LSST::M1M3::TS::Settings;

Heaters::Heaters(token) { pRange = 1; }

void Heaters::load(YAML::Node doc) {
    SPDLOG_TRACE("Loading heaters settigns");
    try {
        pRange = doc["PRange"].as<float>(1.0);
        if (pRange <= 0) {
            throw std::runtime_error("Heaters/PRange must be greater than 0");
        }

        interval = doc["Interval"].as<float>();
        if (interval <= 0) {
            throw std::runtime_error("Heaters/Interval must be greater than 0");
        }
    } catch (YAML::Exception &ex) {
        throw std::runtime_error(fmt::format("Cannot load Heaters settings: {}", ex.what()));
    }
}
