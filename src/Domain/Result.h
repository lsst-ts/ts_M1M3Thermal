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

#ifndef DOMAIN_RESULT_H_
#define DOMAIN_RESULT_H_

#include <string>

namespace LSST {
namespace TS {
namespace MTM1M3TS {

template <class resultType>
class Result {
public:
    bool Valid;
    resultType Value;
    std::string Description;

    Result(resultType value = resultType(), bool valid = true, std::string description = "") {
        this->Valid = valid;
        this->Value = value;
        this->Description = description;
    }
};

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */

#endif /* DOMAIN_RESULT_H_ */