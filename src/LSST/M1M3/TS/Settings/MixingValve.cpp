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

#include <Settings/MixingValve.h>

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

using namespace LSST::M1M3::TS::Settings;

void MixingValve::load(const std::string &filename) {
    SPDLOG_DEBUG("MixingValve::load(\"{}\")", filename);

    try {
        YAML::Node doc = YAML::LoadFile(filename);

        commandingFullyOpen = doc["Commanding"]["FullyOpen"].as<float>();

        positionFeedbackFullyOpen = doc["PositionFeedback"]["FullyOpen"].as<float>();
    } catch (YAML::Exception &ex) {
        throw std::runtime_error(fmt::format("YAML Loading {}: {}", filename, ex.what()));
    }
}

float MixingValve::percentsToCommanded(float target) {
    return (commandingFullyOpen / 1000.0f) * (target / 100.0f);
}

float MixingValve::positionToPercents(float position) {
    float ret = 100.0f * (position / positionFeedbackFullyOpen);
    return std::max(0.0f, std::min(100.0f, ret));
}
