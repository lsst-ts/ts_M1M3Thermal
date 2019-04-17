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
#include "../src/Domain/Functions.h"

using namespace LSST::TS::MTM1M3TS;

TEST_CASE("Linear Function", "[Functions]") {
	LinearFunction uut = LinearFunction(1.2, 2.3);

	REQUIRE(uut.evaluate(0.0) == Approx(2.3));
	REQUIRE(uut.evaluate(1.0) == Approx(3.5));
	REQUIRE(uut.evaluate(-1.0) == Approx(1.1));
}

TEST_CASE("Poly 5 Function", "[Functions]") {
	Poly5Function uut = Poly5Function(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);

	REQUIRE(uut.evaluate(0.0) == Approx(6.0));
	REQUIRE(uut.evaluate(0.5) == Approx(10.03125));
}

TEST_CASE("Any Bit Set Function", "[Functions]") {
	AnyBitSetFunction uut = AnyBitSetFunction(0x30);

	REQUIRE(uut.evaluate(0x01) == false);
	REQUIRE(uut.evaluate(0x10) == true);
	REQUIRE(uut.evaluate(0x20) == true);
	REQUIRE(uut.evaluate(0x30) == true);
}

TEST_CASE("All Bit Set Function", "[Functions]") {
	AllBitSetFunction uut = AllBitSetFunction(0x30);

	REQUIRE(uut.evaluate(0x01) == false);
	REQUIRE(uut.evaluate(0x10) == false);
	REQUIRE(uut.evaluate(0x20) == false);
	REQUIRE(uut.evaluate(0x30) == true);
}

TEST_CASE("Any Bit Not Set Function", "[Functions]") {
	AnyBitNotSetFunction uut = AnyBitNotSetFunction(0x30);

	REQUIRE(uut.evaluate(0x01) == true);
	REQUIRE(uut.evaluate(0x10) == true);
	REQUIRE(uut.evaluate(0x20) == true);
	REQUIRE(uut.evaluate(0x30) == false);
}

TEST_CASE("All Bit Not Set Function", "[Functions]") {
	AllBitNotSetFunction uut = AllBitNotSetFunction(0x30);

	REQUIRE(uut.evaluate(0x01) == true);
	REQUIRE(uut.evaluate(0x10) == false);
	REQUIRE(uut.evaluate(0x20) == false);
	REQUIRE(uut.evaluate(0x30) == false);
}