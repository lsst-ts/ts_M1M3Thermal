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

#ifndef DOMAIN_LIMITS_H_
#define DOMAIN_LIMITS_H_

#include <vector>

#include "Defines.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

/******************************************************************************
 * Limits
 *****************************************************************************/

enum class Limits {
	UNKNOWN = 0,
	OK = 1,
	WARNING = 2,
	FAULT = 3,
	BYPASSED_OK = 4,
	BYPASSED_WARNING = 5,
	BYPASSED_FAULT = 6
};

static inline Limits mergeLimits(Limits a, Limits b) { return a >= b ? a : b; }

/******************************************************************************
 * Not Equal Limit
 *****************************************************************************/

class NotEqualLimit {
private:
	int64_t threshold;
	Limits limit;

public:
	NotEqualLimit(int64_t threshold = 0, Limits limit = Limits::OK);

	inline Limits evaluate(int64_t x) {
		return x != this->threshold ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * Equal Limit
 *****************************************************************************/

class EqualLimit {
private:
	int64_t threshold;
	Limits limit;

public:
	EqualLimit(int64_t threshold = 0, Limits limit = Limits::OK);

	inline Limits evaluate(int64_t x) {
		return x == this->threshold ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * Less Than Limit
 *****************************************************************************/

class LessThanLimit {
private:
	float threshold;
	Limits limit;

public:
	LessThanLimit(float threshold = 0.0, Limits limit = Limits::OK);

	inline Limits evaluate(float x) {
		return x < this->threshold ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * Less Than Equal Limit
 *****************************************************************************/

class LessThanEqualLimit {
private:
	float threshold;
	Limits limit;

public:
	LessThanEqualLimit(float threshold = 0.0, Limits limit = Limits::OK);

	inline Limits evaluate(float x) {
		return x <= this->threshold ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * Greater Than Limit
 *****************************************************************************/

class GreaterThanLimit {
private:
	float threshold;
	Limits limit;

public:
	GreaterThanLimit(float threshold = 0.0, Limits limit = Limits::OK);

	inline Limits evaluate(float x) {
		return x > this->threshold ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * Greater Than Equal Limit
 *****************************************************************************/

class GreaterThanEqualLimit {
private:
	float threshold;
	Limits limit;

public:
	GreaterThanEqualLimit(float threshold = 0.0, Limits limit = Limits::OK);

	inline Limits evaluate(float x) {
		return x >= this->threshold ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * Not In Range Limit
 *****************************************************************************/

class NotInRangeLimit {
private:
	float minValue;
	float maxValue;
	Limits limit;

public:
	NotInRangeLimit(float minValue = 0.0, float maxValue = 0.0, Limits limit = Limits::OK);

	inline Limits evaluate(float x) {
		return x < this->minValue || x > this->maxValue ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * In Range Limit
 *****************************************************************************/

class InRangeLimit {
private:
	float minValue;
	float maxValue;
	Limits limit;

public:
	InRangeLimit(float minValue = 0.0, float maxValue = 0.0, Limits limit = Limits::OK);

	inline Limits evaluate(float x) {
		return x >= this->minValue && x <= this->maxValue ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * Not In Tolerance Limit
 *****************************************************************************/

class NotInToleranceLimit {
private:
	float minValue;
	float maxValue;
	Limits limit;

public:
	NotInToleranceLimit(float target = 0.0, float tolerance = 0.0, Limits limit = Limits::OK);

	inline Limits evaluate(float x) {
		return x < this->minValue || x > this->maxValue ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * In Tolerance Limit
 *****************************************************************************/

class InToleranceLimit {
private:
	float minValue;
	float maxValue;
	Limits limit;

public:
	InToleranceLimit(float target = 0.0, float tolerance = 0.0, Limits limit = Limits::OK);

	inline Limits evaluate(float x) {
		return x >= this->minValue && x <= this->maxValue ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * Any Bit Set Limit
 *****************************************************************************/

class AnyBitSetLimit {
private:
	uint64_t mask;
	Limits limit;

public:
	AnyBitSetLimit(uint64_t mask = 0, Limits limit = Limits::OK);

	inline Limits evaluate(uint64_t x) {
		return (x & this->mask) != 0 ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * All Bit Set Limit
 *****************************************************************************/

class AllBitSetLimit {
private:
	uint64_t mask;
	Limits limit;

public:
	AllBitSetLimit(uint64_t mask = 0, Limits limit = Limits::OK);

	inline Limits evaluate(uint64_t x) {
		return (x & this->mask) == this->mask ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * Any Bit Not Set Limit
 *****************************************************************************/

class AnyBitNotSetLimit {
private:
	uint64_t mask;
	Limits limit;

public:
	AnyBitNotSetLimit(uint64_t mask = 0, Limits limit = Limits::OK);

	inline Limits evaluate(uint64_t x) {
		return (x & this->mask) != this->mask ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * All Bit Not Set Limit
 *****************************************************************************/

class AllBitNotSetLimit {
private:
	uint64_t mask;
	Limits limit;

public:
	AllBitNotSetLimit(uint64_t mask = 0, Limits limit = Limits::OK);

	inline Limits evaluate(uint64_t x) {
		return (x & this->mask) == 0 ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * Continuous Timed Limit
 *****************************************************************************/

class ContinuousTimedLimit {
private:
	uint32_t threshold;
	Limits limit;

	uint32_t count;

public:
	ContinuousTimedLimit(uint32_t threshold = 0, Limits limit = Limits::OK);

	inline void reset() {
		this->count = 0;
	}

	inline Limits evaluate(Limits x) {
		this->count = x >= Limits::WARNING ? this->count + 1 : 0;
		return this->count >= this->threshold ? this->limit : Limits::OK;
	}
};

/******************************************************************************
 * Timed Limit
 *****************************************************************************/

class TimedLimit {
private:
	uint32_t duration;
	uint32_t threshold;
	Limits limit;

	std::vector<Limits> history;
	uint32_t count;
	uint32_t index;

public:
	TimedLimit(uint32_t duration = 1, uint32_t threshold = 0, Limits limit = Limits::OK);

	void reset();

	Limits evaluate(Limits x);
};


} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */

#endif /* DOMAIN_LIMITS_H_ */
