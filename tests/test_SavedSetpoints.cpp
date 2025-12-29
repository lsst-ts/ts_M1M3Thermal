/*
 * This file is part of LSST cRIOcpp test suite. Tests MixingValve settings.
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

#include <filesystem>
#include <time.h>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <SAL_MTM1M3TS.h>

#include <cRIO/Settings/Path.h>

#include "Settings/SavedSetpoints.h"
#include "TSPublisher.h"

using Catch::Approx;
using namespace LSST::M1M3::TS;
using namespace LSST::M1M3::TS::Settings;

void init_sal() {
    std::shared_ptr<SAL_MTM1M3TS> m1m3TSSAL = std::make_shared<SAL_MTM1M3TS>();
    m1m3TSSAL->setDebugLevel(2);
    TSPublisher::instance().setSAL(m1m3TSSAL);

    LSST::cRIO::Settings::Path::setRootPath("data");
}

TEST_CASE("Test setpoints saving and recovery", "[SavedSetpoints]") {
    init_sal();

    SavedSetpoints settings("_setpoints.yaml");
    std::filesystem::remove(settings.file_path);

    REQUIRE(settings.is_valid() == false);

    REQUIRE_NOTHROW(settings.load());

    REQUIRE(settings.is_valid() == false);
    REQUIRE(settings.date().tm_year == 0);

    REQUIRE_NOTHROW(settings.save(4, 5));
    REQUIRE_NOTHROW(settings.load());

    REQUIRE(settings.is_valid() == true);
    REQUIRE(settings.glycol() == 4);
    REQUIRE(settings.heaters() == 5);

    auto now = time(nullptr);
    auto file_date = settings.date();

    auto t_delta = difftime(now, mktime(&file_date));

    REQUIRE(t_delta < 1);
    REQUIRE(t_delta >= 0);

    REQUIRE_NOTHROW(std::filesystem::remove(settings.file_path) == true);
}

TEST_CASE("Test setpoints too old date loading", "[SavedSetpoints]") {
    init_sal();

    SavedSetpoints settings("wrong_date_settings.yaml");

    REQUIRE_NOTHROW(settings.load());

    REQUIRE(settings.is_valid() == false);
    REQUIRE(isnan(settings.glycol()));
    REQUIRE(isnan(settings.heaters()));
}
