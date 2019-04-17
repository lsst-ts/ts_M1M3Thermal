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

#include "Limits.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

NotEqualLimit::NotEqualLimit(int64_t threshold, Limits limit) {
	this->threshold = threshold;
	this->limit = limit;
}

EqualLimit::EqualLimit(int64_t threshold, Limits limit) {
	this->threshold = threshold;
	this->limit = limit;
}

LessThanLimit::LessThanLimit(float threshold, Limits limit) {
	this->threshold = threshold;
	this->limit = limit;
}

LessThanEqualLimit::LessThanEqualLimit(float threshold, Limits limit) {
	this->threshold = threshold;
	this->limit = limit;
}

GreaterThanLimit::GreaterThanLimit(float threshold, Limits limit) {
	this->threshold = threshold;
	this->limit = limit;
}

GreaterThanEqualLimit::GreaterThanEqualLimit(float threshold, Limits limit) {
	this->threshold = threshold;
	this->limit = limit;
}

NotInRangeLimit::NotInRangeLimit(float minValue, float maxValue, Limits limit) {
	this->minValue = minValue;
	this->maxValue = maxValue;
	this->limit = limit;
}

InRangeLimit::InRangeLimit(float minValue, float maxValue, Limits limit) {
	this->minValue = minValue;
	this->maxValue = maxValue;
	this->limit = limit;
}

NotInToleranceLimit::NotInToleranceLimit(float target, float tolerance, Limits limit) {
	this->minValue = target - tolerance;
	this->maxValue = target + tolerance;
	this->limit = limit;
}

InToleranceLimit::InToleranceLimit(float target, float tolerance, Limits limit) {
	this->minValue = target - tolerance;
	this->maxValue = target + tolerance;
	this->limit = limit;
}

AnyBitSetLimit::AnyBitSetLimit(uint64_t mask, Limits limit) {
	this->mask = mask;
	this->limit = limit;
}

AllBitSetLimit::AllBitSetLimit(uint64_t mask, Limits limit) {
	this->mask = mask;
	this->limit = limit;
}

AnyBitNotSetLimit::AnyBitNotSetLimit(uint64_t mask, Limits limit) {
	this->mask = mask;
	this->limit = limit;
}

AllBitNotSetLimit::AllBitNotSetLimit(uint64_t mask, Limits limit) {
	this->mask = mask;
	this->limit = limit;
}

ContinuousTimedLimit::ContinuousTimedLimit(uint32_t threshold, Limits limit) {
	this->threshold = threshold;
	this->limit = limit;

	this->count = 0;
}

TimedLimit::TimedLimit(uint32_t duration, uint32_t threshold, Limits limit) {
	this->duration = duration;
	this->threshold = threshold;
	this->limit = limit;

	this->history.reserve(this->duration);
	for(uint32_t i = 0; i < this->duration; ++i) {
		this->history.push_back(Limits::OK);
	}

	this->count = 0;
	this->index = 0;
}

void TimedLimit::reset() {
	this->count = 0;
	for(uint32_t i = 0; i < this->duration; ++i) {
		this->history[i] = Limits::OK;
	}
}

Limits TimedLimit::evaluate(Limits x) {
	this->count -= this->history[this->index] >= Limits::WARNING ? 1 : 0;
	this->history[this->index] = x;
	this->count += this->history[this->index] >= Limits::WARNING ? 1 : 0;
	++this->index;
	if (this->index >= this->duration) {
		this->index = 0;
	}
	return this->count >= this->threshold ? this->limit : Limits::OK;
}

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */
