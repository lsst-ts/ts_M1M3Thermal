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

#include "catch.hpp"
#include "../src/Domain/Limits.h"

using namespace LSST::TS::MTM1M3TS;

TEST_CASE("Merge Limits", "[Limits]") {
	REQUIRE(mergeLimits(Limits::UNKNOWN, Limits::OK) == Limits::OK);
	REQUIRE(mergeLimits(Limits::OK, Limits::WARNING) == Limits::WARNING);
	REQUIRE(mergeLimits(Limits::WARNING, Limits::FAULT) == Limits::FAULT);
	REQUIRE(mergeLimits(Limits::FAULT, Limits::UNKNOWN) == Limits::FAULT);
}

TEST_CASE("Not Equal Limit", "[Limits]") {
	NotEqualLimit uut = NotEqualLimit(1, Limits::WARNING);
	
	REQUIRE(uut.evaluate(1) == Limits::OK);
	REQUIRE(uut.evaluate(0) == Limits::WARNING);
}

TEST_CASE("Equal Limit", "[Limits]") {
	EqualLimit uut = EqualLimit(2, Limits::FAULT);

	REQUIRE(uut.evaluate(0) == Limits::OK);
	REQUIRE(uut.evaluate(2) == Limits::FAULT);
}

TEST_CASE("Less Than Limit", "[Limits]") {
	LessThanLimit uut = LessThanLimit(1, Limits::WARNING);

	REQUIRE(uut.evaluate(2) == Limits::OK);
	REQUIRE(uut.evaluate(1) == Limits::OK);
	REQUIRE(uut.evaluate(0) == Limits::WARNING);
}

TEST_CASE("Less Than Equal Limit", "[Limits]") {
	LessThanEqualLimit uut = LessThanEqualLimit(2, Limits::FAULT);

	REQUIRE(uut.evaluate(3) == Limits::OK);
	REQUIRE(uut.evaluate(2) == Limits::FAULT);
	REQUIRE(uut.evaluate(1) == Limits::FAULT);
}

TEST_CASE("Greater Than Limit", "[Limits]") {
	GreaterThanLimit uut = GreaterThanLimit(1, Limits::WARNING);

	REQUIRE(uut.evaluate(0) == Limits::OK);
	REQUIRE(uut.evaluate(1) == Limits::OK);
	REQUIRE(uut.evaluate(2) == Limits::WARNING);
}

TEST_CASE("Greater Than Equal Limit", "[Limits]") {
	GreaterThanEqualLimit uut = GreaterThanEqualLimit(2, Limits::FAULT);

	REQUIRE(uut.evaluate(1) == Limits::OK);
	REQUIRE(uut.evaluate(2) == Limits::FAULT);
	REQUIRE(uut.evaluate(3) == Limits::FAULT);
}

TEST_CASE("Not In Range Limit", "[Limits]") {
	NotInRangeLimit uut = NotInRangeLimit(-1, 1, Limits::WARNING);

	REQUIRE(uut.evaluate(-2) == Limits::WARNING);
	REQUIRE(uut.evaluate(-1) == Limits::OK);
	REQUIRE(uut.evaluate(0) == Limits::OK);
	REQUIRE(uut.evaluate(1) == Limits::OK);
	REQUIRE(uut.evaluate(2) == Limits::WARNING);
}

TEST_CASE("In Range Limit", "[Limits]") {
	InRangeLimit uut = InRangeLimit(-1, 1, Limits::FAULT);

	REQUIRE(uut.evaluate(-2) == Limits::OK);
	REQUIRE(uut.evaluate(-1) == Limits::FAULT);
	REQUIRE(uut.evaluate(0) == Limits::FAULT);
	REQUIRE(uut.evaluate(1) == Limits::FAULT);
	REQUIRE(uut.evaluate(2) == Limits::OK);
}

TEST_CASE("Not In Tolerance Limit", "[Limits]") {
	NotInToleranceLimit uut = NotInToleranceLimit(0, 1, Limits::WARNING);

	REQUIRE(uut.evaluate(-2) == Limits::WARNING);
	REQUIRE(uut.evaluate(-1) == Limits::OK);
	REQUIRE(uut.evaluate(0) == Limits::OK);
	REQUIRE(uut.evaluate(1) == Limits::OK);
	REQUIRE(uut.evaluate(2) == Limits::WARNING);
}

TEST_CASE("In Tolerance Limit", "[Limits]") {
	InToleranceLimit uut = InToleranceLimit(0, 1, Limits::FAULT);

	REQUIRE(uut.evaluate(-2) == Limits::OK);
	REQUIRE(uut.evaluate(-1) == Limits::FAULT);
	REQUIRE(uut.evaluate(0) == Limits::FAULT);
	REQUIRE(uut.evaluate(1) == Limits::FAULT);
	REQUIRE(uut.evaluate(2) == Limits::OK);
}

TEST_CASE("Any Bit Set Limit", "[Limits]") {
	AnyBitSetLimit uut = AnyBitSetLimit(0x30, Limits::WARNING);

	REQUIRE(uut.evaluate(0x01) == Limits::OK);
	REQUIRE(uut.evaluate(0x10) == Limits::WARNING);
	REQUIRE(uut.evaluate(0x20) == Limits::WARNING);
	REQUIRE(uut.evaluate(0x30) == Limits::WARNING);
}

TEST_CASE("All Bit Set Limit", "[Limits]") {
	AllBitSetLimit uut = AllBitSetLimit(0x30, Limits::FAULT);

	REQUIRE(uut.evaluate(0x01) == Limits::OK);
	REQUIRE(uut.evaluate(0x10) == Limits::OK);
	REQUIRE(uut.evaluate(0x20) == Limits::OK);
	REQUIRE(uut.evaluate(0x30) == Limits::FAULT);
}

TEST_CASE("Any Bit Not Set Limit", "[Limits]") {
	AnyBitNotSetLimit uut = AnyBitNotSetLimit(0x30, Limits::WARNING);

	REQUIRE(uut.evaluate(0x01) == Limits::WARNING);
	REQUIRE(uut.evaluate(0x10) == Limits::WARNING);
	REQUIRE(uut.evaluate(0x20) == Limits::WARNING);
	REQUIRE(uut.evaluate(0x30) == Limits::OK);
}

TEST_CASE("All Bit Not Set Limit", "[Limits]") {
	AllBitNotSetLimit uut = AllBitNotSetLimit(0x30, Limits::FAULT);

	REQUIRE(uut.evaluate(0x01) == Limits::FAULT);
	REQUIRE(uut.evaluate(0x10) == Limits::OK);
	REQUIRE(uut.evaluate(0x20) == Limits::OK);
	REQUIRE(uut.evaluate(0x30) == Limits::OK);
}

TEST_CASE("Continuous Timed Limit", "[Limits]") {
	ContinuousTimedLimit uut = ContinuousTimedLimit(3, Limits::WARNING);

	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::OK);
}

TEST_CASE("Timed Limit", "[Limits]") {
	TimedLimit uut = TimedLimit(4, 2, Limits::WARNING);

	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::WARNING);
	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::OK);
	REQUIRE(uut.evaluate(Limits::WARNING) == Limits::WARNING);

	uut.reset();

	REQUIRE(uut.evaluate(Limits::OK) == Limits::OK);
}
