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

#include "Functions.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

/******************************************************************************
 * Linear Function
 *****************************************************************************/

LinearFunction::LinearFunction(float m, float b) {
	this->m = m;
	this->b = b;
}

/******************************************************************************
 * Poly 5 Function
 *****************************************************************************/

Poly5Function::Poly5Function(float a, float b, float c, float d, float e, float f) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
	this->e = e;
	this->f = f;
}

/******************************************************************************
 * Any Bit Set Function
 *****************************************************************************/

AnyBitSetFunction::AnyBitSetFunction(uint64_t mask) {
	this->mask = mask;
}

/******************************************************************************
 * All Bit Set Function
 *****************************************************************************/

AllBitSetFunction::AllBitSetFunction(uint64_t mask) {
	this->mask = mask;
}

/******************************************************************************
 * Any Bit Not Set Function
 *****************************************************************************/

AnyBitNotSetFunction::AnyBitNotSetFunction(uint64_t mask) {
	this->mask = mask;
}

/******************************************************************************
 * All Bit Not Set Function
 *****************************************************************************/

AllBitNotSetFunction::AllBitNotSetFunction(uint64_t mask) {
	this->mask = mask;
}

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */
