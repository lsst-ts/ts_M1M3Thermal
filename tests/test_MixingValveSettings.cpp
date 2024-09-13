/*
 * This file is part of LSST cRIOcpp test suite. Tests MixingVakve settings.
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

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <SAL_MTM1M3TS.h>

#include <cRIO/Settings/Path.h>

#include <Settings/Controller.h>
#include <Settings/MixingValve.h>
#include <TSPublisher.h>

using Catch::Approx;
using namespace LSST::M1M3::TS;
using namespace LSST::M1M3::TS::Settings;

void init_sal() {
    std::shared_ptr<SAL_MTM1M3TS> m1m3TSSAL = std::make_shared<SAL_MTM1M3TS>();
    m1m3TSSAL->setDebugLevel(2);
    TSPublisher::instance().setSAL(m1m3TSSAL);
}

TEST_CASE("Test conversions", "[MixingValveSettings]") {
    init_sal();

    LSST::cRIO::Settings::Path::setRootPath("data");
    REQUIRE_NOTHROW(Controller::instance().load("data/_init.yaml"));

    REQUIRE(MixingValve::instance().positionToPercents(10) == 100);
    REQUIRE(MixingValve::instance().positionToPercents(-1) == 0);

    REQUIRE(MixingValve::instance().percentsToCommanded(100) == Approx(0.020f));
    REQUIRE(MixingValve::instance().percentsToCommanded(50) == Approx(0.012f));
}
