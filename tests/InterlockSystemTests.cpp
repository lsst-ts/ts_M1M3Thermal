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
#include "../src/InterlockSystem/InterlockSystem.h"
#include "../src/Domain/Result.h"
#include "../src/Utility/Timestamp.h"

using namespace LSST::TS::MTM1M3TS;

TEST_CASE("Interlock System", "[InterlockSystem]") {
    InterlockSystemSettings settings;
    settings.RawSampleToFanCoilHeatersOffInterlock = AllBitNotSetFunction(0x00010000);
    settings.RawSampleToCoolantPumpOffInterlock = AllBitNotSetFunction(0x00020000);
    settings.RawSampleToGISHeartbeatLostInterlock = AllBitNotSetFunction(0x00040000);
    settings.RawSampleToMixingValveClosedInterlock = AllBitNotSetFunction(0x00080000);
    settings.RawSampleToSupportSystemHeartbeatLostInterlock = AllBitNotSetFunction(0x00100000);
    settings.RawSampleToCellDoorOpenInterlock = AllBitNotSetFunction(0x00200000);
    settings.RawSampleToGISEarthquakeInterlock = AllBitNotSetFunction(0x00400000);
    settings.RawSampleToCoolantPumpEStopInterlock = AllBitNotSetFunction(0x00800000);
    settings.RawSampleToCabinetOverTempInterlock = AllBitNotSetFunction(0x01000000);
    settings.FanCoilHeatersOffInterlockLimit = EqualLimit(1, Limits::WARNING);
    settings.CoolantPumpOffInterlockLimit = EqualLimit(1, Limits::WARNING);
    settings.GISHeartbeatLostInterlockLimit = EqualLimit(1, Limits::WARNING);
    settings.MixingValveClosedInterlockLimit = EqualLimit(1, Limits::WARNING);
    settings.SupportSystemHeartbeatLostInterlockLimit = EqualLimit(1, Limits::WARNING);
    settings.CellDoorOpenInterlockLimit = EqualLimit(1, Limits::WARNING);
    settings.GISEarthquakeInterlockLimit = EqualLimit(1, Limits::FAULT);
    settings.CoolantPumpEStopInterlockLimit = EqualLimit(1, Limits::FAULT);
    settings.CabinetOverTempInterlockLimit = EqualLimit(1, Limits::WARNING);
    settings.HeartbeatTogglePeriod = 10;
    InterlockSystemData data;
    ThermalFPGASystemData thermalFPGASystemData;
    InterlockSystemModel model = InterlockSystemModel(settings, data, thermalFPGASystemData);
    InterlockSystemContext context = InterlockSystemContext(model);
    InterlockSystem system = InterlockSystem(context);

    REQUIRE(data.State == InterlockSystemStates::Standby);

    SECTION("Standby State") {

        SECTION("from standby state verify standby command doesn't change state") {
            Result<SubsystemResults> result = system.standby();

            REQUIRE(result.Valid == false);
            REQUIRE(result.Value == SubsystemResults::InvalidState);
            REQUIRE(result.Description == "The interlock system cannot execute the standby command from the Standby state.");
            REQUIRE(data.State == InterlockSystemStates::Standby);
        }

        SECTION("from standby state verify start command changes state to disabled state") {
            Result<SubsystemResults> result = system.start();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Disabled);
        }

        SECTION("from standby state verify update command shows all ok") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(1.23);
            thermalFPGASystemData.DigitalInputStates = 0x01FF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
            REQUIRE(data.SampleTimestamp == Approx(1.23));
            REQUIRE(data.RawSample == 0x01FF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from standby state verify update command shows bad fan coil heaters off interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(2.34);
            thermalFPGASystemData.DigitalInputStates = 0x01FE0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
            REQUIRE(data.SampleTimestamp == Approx(2.34));
            REQUIRE(data.RawSample == 0x01FE0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == true);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::WARNING);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from standby state verify update command shows bad coolant pump off interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(3.45);
            thermalFPGASystemData.DigitalInputStates = 0x01FD0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
            REQUIRE(data.SampleTimestamp == Approx(3.45));
            REQUIRE(data.RawSample == 0x01FD0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == true);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::WARNING);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from standby state verify update command shows bad GIS heartbeat lost interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(4.56);
            thermalFPGASystemData.DigitalInputStates = 0x01FB0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
            REQUIRE(data.SampleTimestamp == Approx(4.56));
            REQUIRE(data.RawSample == 0x01FB0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == true);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::WARNING);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from standby state verify update command shows bad mixing valve closed interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(5.67);
            thermalFPGASystemData.DigitalInputStates = 0x01F70000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
            REQUIRE(data.SampleTimestamp == Approx(5.67));
            REQUIRE(data.RawSample == 0x01F70000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == true);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::WARNING);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from standby state verify update command shows bad support system heartbeat lost interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(6.78);
            thermalFPGASystemData.DigitalInputStates = 0x01EF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
            REQUIRE(data.SampleTimestamp == Approx(6.78));
            REQUIRE(data.RawSample == 0x01EF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == true);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::WARNING);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from standby state verify update command shows bad cell door open interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(7.89);
            thermalFPGASystemData.DigitalInputStates = 0x01DF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
            REQUIRE(data.SampleTimestamp == Approx(7.89));
            REQUIRE(data.RawSample == 0x01DF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == true);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::WARNING);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from standby state verify update command shows bad GIS earthquake interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(10.1112);
            thermalFPGASystemData.DigitalInputStates = 0x01BF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
            REQUIRE(data.SampleTimestamp == Approx(10.1112));
            REQUIRE(data.RawSample == 0x01BF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == true);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::FAULT);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from standby state verify update command shows bad coolant pump e-stop interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(11.1213);
            thermalFPGASystemData.DigitalInputStates = 0x017F0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
            REQUIRE(data.SampleTimestamp == Approx(11.1213));
            REQUIRE(data.RawSample == 0x017F0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == true);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::FAULT);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from standby state verify update command shows bad cabinet over temp interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(12.1314);
            thermalFPGASystemData.DigitalInputStates = 0x00FF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
            REQUIRE(data.SampleTimestamp == Approx(12.1314));
            REQUIRE(data.RawSample == 0x00FF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == true);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::WARNING);
        }

        SECTION("from standby state verify update command toggles heartbeat") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(1.23);
            thermalFPGASystemData.DigitalInputStates = 0x001FF0000;
            
            data.HeartbeatCommand = false;

            for(uint32_t i = 0; i < settings.HeartbeatTogglePeriod - 1; ++i) {
                system.update();
            }

            REQUIRE(data.HeartbeatCommand == false);

            system.update();

            REQUIRE(data.HeartbeatCommand == true);
        }
    }
    
    SECTION("Disabled State") {
        system.start();

        REQUIRE(data.State == InterlockSystemStates::Disabled);

        SECTION("from disabled state verify standby command changes state to standby state") {
            Result<SubsystemResults> result = system.standby();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
        }

        SECTION("from disabled state verify start command doesn't change state") {
            Result<SubsystemResults> result = system.start();

            REQUIRE(result.Valid == false);
            REQUIRE(result.Value == SubsystemResults::InvalidState);
            REQUIRE(result.Description == "The interlock system cannot execute the start command from the Disabled state.");
            REQUIRE(data.State == InterlockSystemStates::Disabled);
        }

        SECTION("from disabled state verify update command shows all ok") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(1.23);
            thermalFPGASystemData.DigitalInputStates = 0x01FF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Disabled);
            REQUIRE(data.SampleTimestamp == Approx(1.23));
            REQUIRE(data.RawSample == 0x01FF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from disabled state verify update command shows bad fan coil heaters off interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(2.34);
            thermalFPGASystemData.DigitalInputStates = 0x01FE0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Disabled);
            REQUIRE(data.SampleTimestamp == Approx(2.34));
            REQUIRE(data.RawSample == 0x01FE0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == true);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::WARNING);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from disabled state verify update command shows bad coolant pump off interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(3.45);
            thermalFPGASystemData.DigitalInputStates = 0x01FD0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Disabled);
            REQUIRE(data.SampleTimestamp == Approx(3.45));
            REQUIRE(data.RawSample == 0x01FD0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == true);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::WARNING);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from disabled state verify update command shows bad GIS heartbeat lost interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(4.56);
            thermalFPGASystemData.DigitalInputStates = 0x01FB0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Disabled);
            REQUIRE(data.SampleTimestamp == Approx(4.56));
            REQUIRE(data.RawSample == 0x01FB0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == true);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::WARNING);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from disabled state verify update command shows bad mixing valve closed interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(5.67);
            thermalFPGASystemData.DigitalInputStates = 0x01F70000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Disabled);
            REQUIRE(data.SampleTimestamp == Approx(5.67));
            REQUIRE(data.RawSample == 0x01F70000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == true);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::WARNING);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from disabled state verify update command shows bad support system heartbeat lost interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(6.78);
            thermalFPGASystemData.DigitalInputStates = 0x01EF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Disabled);
            REQUIRE(data.SampleTimestamp == Approx(6.78));
            REQUIRE(data.RawSample == 0x01EF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == true);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::WARNING);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from disabled state verify update command shows bad cell door open interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(7.89);
            thermalFPGASystemData.DigitalInputStates = 0x01DF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Disabled);
            REQUIRE(data.SampleTimestamp == Approx(7.89));
            REQUIRE(data.RawSample == 0x01DF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == true);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::WARNING);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from disabled state verify update command shows bad GIS earthquake interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(10.1112);
            thermalFPGASystemData.DigitalInputStates = 0x01BF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Fault);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(10.1112));
            REQUIRE(data.RawSample == 0x01BF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == true);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::FAULT);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from disabled state verify update command shows bad coolant pump e-stop interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(11.1213);
            thermalFPGASystemData.DigitalInputStates = 0x017F0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Fault);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(11.1213));
            REQUIRE(data.RawSample == 0x017F0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == true);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::FAULT);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from disabled state verify update command shows bad cabinet over temp interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(12.1314);
            thermalFPGASystemData.DigitalInputStates = 0x00FF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Disabled);
            REQUIRE(data.SampleTimestamp == Approx(12.1314));
            REQUIRE(data.RawSample == 0x00FF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == true);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::WARNING);
        }

        SECTION("from disabled state verify update command toggles heartbeat") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(1.23);
            thermalFPGASystemData.DigitalInputStates = 0x001FF0000;
            
            data.HeartbeatCommand = false;

            for(uint32_t i = 0; i < settings.HeartbeatTogglePeriod - 1; ++i) {
                system.update();
            }

            REQUIRE(data.HeartbeatCommand == false);

            system.update();

            REQUIRE(data.HeartbeatCommand == true);
        }
    }

    SECTION("Fault State") {
        system.start();

        REQUIRE(data.State == InterlockSystemStates::Disabled);

        thermalFPGASystemData.DigitalInputStates = 0x005B;
        system.update();

        REQUIRE(data.State == InterlockSystemStates::Fault);

        SECTION("from fault state verify standby command changes state to standby state") {
            Result<SubsystemResults> result = system.standby();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Standby);
        }

        SECTION("from fault state verify start command doesn't change state") {
            Result<SubsystemResults> result = system.start();

            REQUIRE(result.Valid == false);
            REQUIRE(result.Value == SubsystemResults::InvalidState);
            REQUIRE(result.Description == "The interlock system cannot execute the start command from the Fault state.");
            REQUIRE(data.State == InterlockSystemStates::Fault);
        }

        SECTION("from fault state verify update command shows all ok") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(1.23);
            thermalFPGASystemData.DigitalInputStates = 0x01FF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(1.23));
            REQUIRE(data.RawSample == 0x01FF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from fault state verify update command shows bad fan coil heaters off interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(2.34);
            thermalFPGASystemData.DigitalInputStates = 0x01FE0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(2.34));
            REQUIRE(data.RawSample == 0x01FE0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == true);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::WARNING);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from fault state verify update command shows bad coolant pump off interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(3.45);
            thermalFPGASystemData.DigitalInputStates = 0x01FD0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(3.45));
            REQUIRE(data.RawSample == 0x01FD0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == true);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::WARNING);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from fault state verify update command shows bad GIS heartbeat lost interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(4.56);
            thermalFPGASystemData.DigitalInputStates = 0x01FB0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(4.56));
            REQUIRE(data.RawSample == 0x01FB0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == true);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::WARNING);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from fault state verify update command shows bad mixing valve closed interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(5.67);
            thermalFPGASystemData.DigitalInputStates = 0x01F70000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(5.67));
            REQUIRE(data.RawSample == 0x01F70000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == true);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::WARNING);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from fault state verify update command shows bad support system heartbeat lost interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(6.78);
            thermalFPGASystemData.DigitalInputStates = 0x01EF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(6.78));
            REQUIRE(data.RawSample == 0x01EF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == true);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::WARNING);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from fault state verify update command shows bad cell door open interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(7.89);
            thermalFPGASystemData.DigitalInputStates = 0x01DF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(7.89));
            REQUIRE(data.RawSample == 0x01DF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == true);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::WARNING);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from fault state verify update command shows bad GIS earthquake interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(10.1112);
            thermalFPGASystemData.DigitalInputStates = 0x01BF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(10.1112));
            REQUIRE(data.RawSample == 0x01BF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == true);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::FAULT);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from fault state verify update command shows bad coolant pump e-stop interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(11.1213);
            thermalFPGASystemData.DigitalInputStates = 0x017F0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(11.1213));
            REQUIRE(data.RawSample == 0x017F0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == true);
            REQUIRE(data.CabinetOverTempInterlock == false);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::FAULT);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::OK);
        }

        SECTION("from fault state verify update command shows bad cabinet over temp interlock") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(12.1314);
            thermalFPGASystemData.DigitalInputStates = 0x00FF0000;

            Result<SubsystemResults> result = system.update();

            REQUIRE(result.Valid == true);
            REQUIRE(result.Value == SubsystemResults::Ok);
            REQUIRE(data.State == InterlockSystemStates::Fault);
            REQUIRE(data.SampleTimestamp == Approx(12.1314));
            REQUIRE(data.RawSample == 0x00FF0000);
            REQUIRE(data.FanCoilHeatersOffInterlock == false);
            REQUIRE(data.CoolantPumpOffInterlock == false);
            REQUIRE(data.GISHeartbeatLostInterlock == false);
            REQUIRE(data.MixingValveClosedInterlock == false);
            REQUIRE(data.SupportSystemHeartbeatLostInterlock == false);
            REQUIRE(data.CellDoorOpenInterlock == false);
            REQUIRE(data.GISEarthquakeInterlock == false);
            REQUIRE(data.CoolantPumpEStopInterlock == false);
            REQUIRE(data.CabinetOverTempInterlock == true);
            REQUIRE(data.FanCoilHeatersOffInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpOffInterlockLimit == Limits::OK);
            REQUIRE(data.GISHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.MixingValveClosedInterlockLimit == Limits::OK);
            REQUIRE(data.SupportSystemHeartbeatLostInterlockLimit == Limits::OK);
            REQUIRE(data.CellDoorOpenInterlockLimit == Limits::OK);
            REQUIRE(data.GISEarthquakeInterlockLimit == Limits::OK);
            REQUIRE(data.CoolantPumpEStopInterlockLimit == Limits::OK);
            REQUIRE(data.CabinetOverTempInterlockLimit == Limits::WARNING);
        }

        SECTION("from fault state verify update command toggles heartbeat") {
            thermalFPGASystemData.DigitalInputSampleTimestamp = Timestamp::toFPGA(1.23);
            thermalFPGASystemData.DigitalInputStates = 0x001FF0000;
            
            data.HeartbeatCommand = false;

            // This is -2 (unlike others) because it takes 1 update to get to
            // the fault state
            for(uint32_t i = 0; i < settings.HeartbeatTogglePeriod - 2; ++i) {
                system.update();
            }

            REQUIRE(data.HeartbeatCommand == false);

            system.update();

            REQUIRE(data.HeartbeatCommand == true);
        }
    }
}