/*
 * This file is part of LSST cRIOcpp test suite. Tests GlycolLoopTemperature task.
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

#include <cmath>

#include <cRIO/Settings/Path.h>

#include "Settings/Controller.h"
#include "Settings/MixingValve.h"
#include "Tasks/GlycolTemperatureControl.h"
#include "Telemetry/FinerControl.h"
#include "TSPublisher.h"

using namespace LSST::M1M3::TS;

void init() {
    std::shared_ptr<SAL_MTM1M3TS> m1m3TSSAL = std::make_shared<SAL_MTM1M3TS>();
    m1m3TSSAL->setDebugLevel(2);
    TSPublisher::instance().setSAL(m1m3TSSAL);

    LSST::cRIO::Settings::Path::setRootPath("data");
    REQUIRE_NOTHROW(Settings::Controller::instance().load("_init.yaml"));
}

TEST_CASE("Finer Controller", "[FinerControl]") {
    init();

    REQUIRE_NOTHROW(Telemetry::FinerControl::instance().set_target(NAN));
    REQUIRE(isnan(Telemetry::FinerControl::instance().get_target(10)));

    REQUIRE_NOTHROW(Telemetry::FinerControl::instance().set_target(4));
    REQUIRE(Telemetry::FinerControl::instance().get_target(4) == 10.0);
    REQUIRE(Telemetry::FinerControl::instance().get_target(4) == 10.0);

    REQUIRE_NOTHROW(Telemetry::FinerControl::instance().set_target(20));
    REQUIRE(Telemetry::FinerControl::instance().get_target(4) == 20.0);
    REQUIRE(Telemetry::FinerControl::instance().get_target(4) == 20.0);
}
