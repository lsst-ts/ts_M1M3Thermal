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

#include "Settings/MixingValve.h"

using namespace LSST::M1M3::TS::Settings;

MixingValve::MixingValve(token) {
    commandingFullyClosed = NAN;
    commandingFullyOpened = NAN;
    positionFeedbackFullyClosed = NAN;
    positionFeedbackFullyOpened = NAN;
}

void MixingValve::load(YAML::Node doc) {
    SPDLOG_INFO("Loading Mixing Valve settigns.");
    commandingFullyClosed = doc["Commanding"]["FullyClosed"].as<float>();
    commandingFullyOpened = doc["Commanding"]["FullyOpened"].as<float>();

    positionFeedbackFullyClosed = doc["PositionFeedback"]["FullyClosed"].as<float>();
    positionFeedbackFullyOpened = doc["PositionFeedback"]["FullyOpened"].as<float>();

    positionFeedbackA = doc["PositionFeedback"]["a"].as<float>();
    positionFeedbackB = doc["PositionFeedback"]["b"].as<float>();

    pid_parameters.load(doc["PID"]);
}

float MixingValve::percents_to_commanded(float target) {
    return (commandingFullyClosed + (commandingFullyOpened - commandingFullyClosed) * (target / 100.0f)) /
           1000.0f;
}

float MixingValve::position_to_percents(float position) {
    if (position < positionFeedbackFullyClosed) {
        return 0;
    }

    if (position > positionFeedbackFullyOpened) {
        return 100;
    }

    float ret = positionFeedbackA + positionFeedbackB * position;
    return std::max(0.0f, std::min(100.0f, ret));
}
