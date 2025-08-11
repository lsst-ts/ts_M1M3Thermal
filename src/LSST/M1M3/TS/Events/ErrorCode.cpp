/*
 * TS ErrorCode.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software
 * Systems. This product includes software developed by the Vera C.Rubin
 * Observatory Project (https://www.lsst.org). See the COPYRIGHT file at the
 * top-level directory of this distribution for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "Events/ErrorCode.h"

using namespace LSST::M1M3::TS;

Events::ErrorCode::ErrorCode(token) { errorCode = -1; }

void Events::ErrorCode::set(int _error_code, const std::string &_error_report,
                            const std::string &_traceback) {
    if (_error_code != errorCode) {
        errorCode = _error_code;
        errorReport = _error_report;
        traceback = _traceback;
        send();
    }
}

void Events::ErrorCode::clear(const std::string &error_code) { set(NoFault, error_code, ""); }
