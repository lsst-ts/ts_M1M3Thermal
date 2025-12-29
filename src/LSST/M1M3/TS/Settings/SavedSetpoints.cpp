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

#include <iomanip>
#include <fstream>
#include <time.h>

#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>

#include <cRIO/Settings/Path.h>

#include "Settings/SavedSetpoints.h"
#include "Settings/Setpoint.h"

using namespace LSST::M1M3::TS::Settings;

SavedSetpoints::SavedSetpoints(std::string filename) {
    file_path = cRIO::Settings::Path::getFilePath("v1/" + filename);
    _glycol = NAN;
    _heaters = NAN;
}

void SavedSetpoints::load() {
    try {
        YAML::Node doc = YAML::LoadFile(file_path);

        auto dat_buf = doc["Date"].as<std::string>();
        auto end = strptime(dat_buf.c_str(), "%Y-%m-%dT%T", &_date);
        if (*end == '0') {
            SPDLOG_WARN("Invalid date in setpoints file {}: {}:", file_path, dat_buf);
            _date.tm_year = 0;
        }
        auto setpoints = doc["Setpoints"];
        _glycol = setpoints["Glycol"].as<float>(NAN);
        _heaters = setpoints["Heaters"].as<float>(NAN);

        if (is_valid()) {
            if (_is_too_old()) {
                SPDLOG_WARN("Recorded setpoints are too old - {}, MaxAge is {} seconds. File {}.", dat_buf,
                            Settings::Setpoint::instance().savedSetpointsMaxAge, file_path);
                _glycol = _heaters = NAN;
            } else {
                SPDLOG_INFO("Loaded setpoints from {}: {:+.2f}°C/{:+.2f}°C", file_path, _glycol, _heaters);
            }
        } else {
            SPDLOG_INFO("Setpoint file {} doesn't contain valid data, ignoring it.", file_path);
        }
    } catch (YAML::Exception &ex) {
        auto msg = fmt::format("Cannot load saved setpoints from {}:{}:{} (line, column): {}", file_path,
                               ex.mark.line, ex.mark.column, ex.what());
        SPDLOG_WARN(msg);
    }
}

void SavedSetpoints::save(float glycol, float heaters) {
    YAML::Node doc, setpoints;

    auto now = time(nullptr);

    gmtime_r(&now, &_date);
    char dat_buf[80];
    strftime(dat_buf, 80, "%FT%T", &_date);

    doc["Date"] = dat_buf;

    setpoints["Glycol"] = glycol;
    setpoints["Heaters"] = heaters;

    doc["Setpoints"] = setpoints;

    std::ofstream ofs(file_path, std::ostream::out | std::ostream::trunc);

    ofs << YAML::Dump(doc);

    ofs.close();

    _glycol = glycol;
    _heaters = heaters;
}

bool SavedSetpoints::is_valid() {
    if (std::isnan(_heaters) || std::isnan(_glycol)) {
        return false;
    }

    return true;
}

bool SavedSetpoints::_is_too_old() {
    auto now = time(nullptr);
    time_t recorded_date = mktime(&_date);

    auto diff = difftime(now, recorded_date);
    return diff > Settings::Setpoint::instance().savedSetpointsMaxAge || diff < -1;
}
