/******************************************************************************
 * This file is part of MTM1M3TS.
 *
 * Developed for the LSST Telescope and Site Systems.
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
 *****************************************************************************/

#ifndef UTILITY_TIMESTAMP_H_
#define UTILITY_TIMESTAMP_H_

#include "../Domain/Defines.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

class Timestamp {
public:
    inline static double fromFPGA(uint64_t timestamp) { return ((double)timestamp) / 1000000000.0; }
    inline static uint64_t toFPGA(double timestamp) { return (uint64_t)(timestamp * 1000000000.0); }
};

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */

#endif /* UTILITY_TIMESTAMP_H_ */