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
#include "../src/Domain/Result.h"
#include "../src/InterlockSystem/InterlockSystem.h"
#include "../src/MainSystem/Commands.h"
#include "../src/MainSystem/MainSystem.h"
#include "../src/Utility/MTM1M3TS.h"
#include "../src/Utility/Timestamp.h"

using namespace LSST::TS::MTM1M3TS;

extern bool salInitialized;
extern SAL_MTM1M3TS sal;

TEST_CASE("Main System", "[MainSystem]") {
    if (!salInitialized) {
        sal = SAL_MTM1M3TS();
        initializeMTM1M3TS(sal);
        salInitialized = true;
    }

    ThermalFPGASystemData thermalFPGAData;
    InterlockSystemSettings interlockSystemSettings;
    InterlockSystemData interlockSystemData;
    InterlockSystemModel interlockSystemModel = InterlockSystemModel(interlockSystemSettings, interlockSystemData, thermalFPGAData);
    InterlockSystemContext interlockSystemContext = InterlockSystemContext(interlockSystemModel);
    InterlockSystem interlockSystem = InterlockSystem(interlockSystemContext);
    MainSystemSettings settings;
    MainSystemData data;
    MainSystemModel model = MainSystemModel(settings, data, interlockSystem);
    MainSystemContext context = MainSystemContext(model);
    MainSystem system = MainSystem(context);

    REQUIRE(data.State == MainSystemStates::Offline);

    SECTION("Offline State") {

        SECTION("from offline state verify boot command") {
            BootCommand boot;
            system.boot(boot);

            REQUIRE(data.State == MainSystemStates::Standby);
        }
    }
    SECTION("Standby State") {
        BootCommand boot;
        system.boot(boot);

        REQUIRE(data.State == MainSystemStates::Standby);

        SECTION("from standby state verify exitControl command") {
            MTM1M3TS_command_exitControlC exitControlData;
            ExitControlCommand exitControl = ExitControlCommand(sal, 2, exitControlData);
            system.exitControl(exitControl);

            REQUIRE(data.State == MainSystemStates::Offline);
        }

        SECTION("from standby state verify start command") {
            MTM1M3TS_command_startC startData;
            StartCommand start = StartCommand(sal, 2, startData);
            system.start(start);

            REQUIRE(data.State == MainSystemStates::Disabled);
        }
    }
    SECTION("Disabled State") {
        BootCommand boot;
        system.boot(boot);

        REQUIRE(data.State == MainSystemStates::Standby);

        MTM1M3TS_command_startC startData;
        StartCommand start = StartCommand(sal, 2, startData);
        system.start(start);

        REQUIRE(data.State == MainSystemStates::Disabled);

        SECTION("from disabled state verify standby command") {
            MTM1M3TS_command_standbyC standbyData;
            StandbyCommand standby = StandbyCommand(sal, 2, standbyData);
            system.standby(standby);

            REQUIRE(data.State == MainSystemStates::Standby);
        }

        SECTION("from disabled state verify enabled command") {
            MTM1M3TS_command_enableC enableData;
            EnableCommand enable = EnableCommand(sal, 2, enableData);
            system.enable(enable);

            REQUIRE(data.State == MainSystemStates::Enabled);
        }
    }
    SECTION("Enabled State") {
        BootCommand boot;
        system.boot(boot);

        REQUIRE(data.State == MainSystemStates::Standby);

        MTM1M3TS_command_startC startData;
        StartCommand start = StartCommand(sal, 2, startData);
        system.start(start);

        REQUIRE(data.State == MainSystemStates::Disabled);

        MTM1M3TS_command_enableC enableData;
        EnableCommand enable = EnableCommand(sal, 2, enableData);
        system.enable(enable);

        REQUIRE(data.State == MainSystemStates::Enabled);

        SECTION("from enabled state verify disable command") {
            MTM1M3TS_command_disableC disableData;
            DisableCommand disable = DisableCommand(sal, 2, disableData);
            system.disable(disable);

            REQUIRE(data.State == MainSystemStates::Disabled);
        }

        SECTION("from enabled state verify enter engineering command") {
            MTM1M3TS_command_enterEngineeringC enterEngineeringData;
            EnterEngineeringCommand enterEngineering = EnterEngineeringCommand(sal, 2, enterEngineeringData);
            system.enterEngineering(enterEngineering);

            REQUIRE(data.State == MainSystemStates::Engineering);
        }
    }
    SECTION("Engineering State") {
        BootCommand boot;
        system.boot(boot);

        REQUIRE(data.State == MainSystemStates::Standby);

        MTM1M3TS_command_startC startData;
        StartCommand start = StartCommand(sal, 2, startData);
        system.start(start);

        REQUIRE(data.State == MainSystemStates::Disabled);

        MTM1M3TS_command_enableC enableData;
        EnableCommand enable = EnableCommand(sal, 2, enableData);
        system.enable(enable);

        REQUIRE(data.State == MainSystemStates::Enabled);

        MTM1M3TS_command_enterEngineeringC enterEngineeringData;
        EnterEngineeringCommand enterEngineering = EnterEngineeringCommand(sal, 2, enterEngineeringData);
        system.enterEngineering(enterEngineering);

        REQUIRE(data.State == MainSystemStates::Engineering);

        SECTION("from engineering state verify exit engineering command") {
            MTM1M3TS_command_exitEngineeringC exitEngineeringData;
            ExitEngineeringCommand exitEngineering = ExitEngineeringCommand(sal, 2, exitEngineeringData);
            system.exitEngineering(exitEngineering);

            REQUIRE(data.State == MainSystemStates::Enabled);
        }
    }
}