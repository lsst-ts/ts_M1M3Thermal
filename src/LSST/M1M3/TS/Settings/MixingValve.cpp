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

#include <Settings/MixingValve.h>

using namespace LSST::M1M3::TS::Settings;

MixingValve::MixingValve(token) {
    commandingFullyClosed = NAN;
    commandingFullyOpened = NAN;
    positionFeedbackFullyClosed = NAN;
    positionFeedbackFullyOpened = NAN;
}

void MixingValve::load(YAML::Node doc) {
    try {
        commandingFullyClosed = doc["Commanding"]["FullyClosed"].as<float>();
        commandingFullyOpened = doc["Commanding"]["FullyOpened"].as<float>();

        positionFeedbackFullyClosed = doc["PositionFeedback"]["FullyClosed"].as<float>();
        positionFeedbackFullyOpened = doc["PositionFeedback"]["FullyOpened"].as<float>();
    } catch (YAML::Exception &ex) {
        throw std::runtime_error(fmt::format("Cannot load Mising Valve settings: {}", ex.what()));
    }
}

float MixingValve::percentsToCommanded(float target) {
    return commandingFullyClosed / 1000.0f +
           ((commandingFullyOpened - commandingFullyClosed) / 1000.0f) * (target / 100.0f);
}

float MixingValve::positionToPercents(float position) {
    float ret = 100.0f * ((position - positionFeedbackFullyClosed) /
                          (positionFeedbackFullyOpened - positionFeedbackFullyClosed));
    return std::max(0.0f, std::min(100.0f, ret));
}
