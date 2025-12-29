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

#ifndef _TS_Settings_SavedSetpoint_h
#define _TS_Settings_SavedSetpoint_h

#include <time.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Settings {

/***
 * Manages saved setpoints. Contains methods to save and load M1M3 TS setpoitns. The setpoints are stored in a
 * yaml file.
 */
class SavedSetpoints {
public:
    /***
     * Contructs saved setpoints instance.
     *
     * @param filename file where setpoints will be saved
     */
    SavedSetpoints(std::string filename);

    void load();
    void save(float glycol, float heaters);

    /***
     * Returns true if a valid data were loaded from the saved file.
     *
     * @return true if valid date were loaded from saved file.
     */
    bool is_valid();

    struct tm date() { return _date; }
    float glycol() { return _glycol; }
    float heaters() { return _heaters; }

    std::string file_path;

private:
    struct tm _date;
    float _glycol;
    float _heaters;

    bool _is_too_old();
};

}  // namespace Settings
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  /// !_TS_Settings_SavedSetpoint_h
