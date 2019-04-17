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

#ifndef DOMAIN_FUNCTIONS_H_
#define DOMAIN_FUNCTIONS_H_

#include "Defines.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

/******************************************************************************
 * Linear Function
 *****************************************************************************/

class LinearFunction {
private:
	float m;
	float b;

public:
	LinearFunction(float m = 0.0, float b = 0.0);

	inline float evaluate(float x) {
		return this->m * x + this->b;
	}
};

/******************************************************************************
 * Poly 5 Function
 *****************************************************************************/

class Poly5Function {
private:
	float a;
	float b;
	float c;
	float d;
	float e;
	float f;

public:
	Poly5Function(float a = 0.0, float b = 0.0, float c = 0.0, float d = 0.0, float e = 0.0, float f = 0.0);

	inline float evaluate(float x) {
		return x * (x * (x * (x * ((x * this->a) + this->b) + this->c) + this->d) + this->e) + this->f;
	}
};

/******************************************************************************
 * Any Bit Set Function
 *****************************************************************************/

class AnyBitSetFunction {
private:
	uint64_t mask;

public:
	AnyBitSetFunction(uint64_t mask = 0);

	inline bool evaluate(uint64_t x) {
		return (x & this->mask) != 0;
	}
};

/******************************************************************************
 * All Bit Set Function
 *****************************************************************************/

class AllBitSetFunction {
private:
	uint64_t mask;

public:
	AllBitSetFunction(uint64_t mask = 0);

	inline bool evaluate(uint64_t x) {
		return (x & this->mask) == this->mask;
	}
};

/******************************************************************************
 * Any Bit Not Set Function
 *****************************************************************************/

class AnyBitNotSetFunction {
private:
	uint64_t mask;

public:
	AnyBitNotSetFunction(uint64_t mask = 0);

	inline bool evaluate(uint64_t x) {
		return (x & this->mask) != this->mask;
	}
};

/******************************************************************************
 * All Bit Not Set Function
 *****************************************************************************/

class AllBitNotSetFunction {
private:
	uint64_t mask;

public:
	AllBitNotSetFunction(uint64_t mask = 0);

	inline bool evaluate(uint64_t x) {
		return (x & this->mask) == 0;
	}
};

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */

#endif /* DOMAIN_FUNCTIONS_H_ */
