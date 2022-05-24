/*
 * This file is part of LSST cRIOcpp test suite. Tests MixingVakve settings.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software Systems.
 * This product includes software developed by the Vera C.Rubin Observatory Project
 * (https://www.lsst.org). See the COPYRIGHT file at the top-level directory of
 * this distribution for details of code ownership.
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

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <Settings/MixingValve.h>

using namespace LSST::M1M3::TS::Settings;
using Catch::Approx;

TEST_CASE("Test conversions", "[MixingValveSettings]") {
    REQUIRE_NOTHROW(MixingValve::instance().load("data/MixingValve.yaml"));

    REQUIRE(MixingValve::instance().positionToPercents(10) == 100);
    REQUIRE(MixingValve::instance().positionToPercents(-1) == 0);

    REQUIRE(MixingValve::instance().percentsToCommanded(100) == Approx(0.020f));
    REQUIRE(MixingValve::instance().percentsToCommanded(50) == Approx(0.012f));
}
