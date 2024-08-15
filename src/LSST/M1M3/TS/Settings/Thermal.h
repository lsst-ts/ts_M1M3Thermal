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

#ifndef _TS_Settings_Thermal_h
#define _TS_Settings_Thermal_h

#include <yaml-cpp/yaml.h>

#include <TSPublisher.h>
#include <cRIO/Singleton.h>

#include <SAL_MTM1M3TS.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Settings {

/**
 * Thermal settings.
 */
class Thermal : public cRIO::Singleton<Thermal>, MTM1M3TS_logevent_thermalSettingsC {
public:
    Thermal(token) {}

    void load(YAML::Node doc);

    void log() { TSPublisher::SAL()->logEvent_thermalSettings(this, 0); }

    bool autoDisable;
    int failuresToDisable;
};

}  // namespace Settings
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  //! _TS_Settings_Thermal_h
