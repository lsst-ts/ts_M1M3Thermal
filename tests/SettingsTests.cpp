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

#include <string>
#include <vector>

#include "catch.hpp"
#include "../src/Settings/Settings.h"
#include "yaml-cpp/yaml.h"

using namespace LSST::TS::MTM1M3TS;

TEST_CASE("Settings Get Setting Path", "[Settings]") {
    Settings settings = Settings();

    SECTION("Empty tag") {
        Result<std::string> result = settings.getSettingPath("");
        REQUIRE(result.Valid == false);
    }

    SECTION("Version is not a number") {
        Result<std::string> result = settings.getSettingPath("unittests,a");
        REQUIRE(result.Valid == false);
    }

    SECTION("Name is not valid") {
        Result<std::string> result = settings.getSettingPath("notanameprobably");
        REQUIRE(result.Valid == false);
    }

    SECTION("Version doesn't exist") {
        Result<std::string> result = settings.getSettingPath("unittests,1000");
        REQUIRE(result.Valid == false);
    }

    SECTION("Find specific version") {
        Result<std::string> result = settings.getSettingPath("unittests,1");
        REQUIRE(result.Valid == true);
    }

    SECTION("Find most recent version") {
        Result<std::string> result = settings.getSettingPath("unittests");
        REQUIRE(result.Valid == true);
        int32_t index = result.Value.find_last_of('3');
        REQUIRE(index == (result.Value.length() - 6));
    }
}

TEST_CASE("Settings Read Setting", "[Settings]") {
    Settings settings = Settings();
    YAML::Node doc;

    SECTION("LinearFunction Ok") {
        doc = YAML::Load("Foo.Bar: { m: 1.2, b: 3.0 }");
        Result<LinearFunction> result = settings.readLinearFunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(1.0) == Approx(4.2));
    }

    SECTION("LinearFunction Bad") {
        doc = YAML::Load("Foo.Bar: { m: 1.2 }");
        Result<LinearFunction> result = settings.readLinearFunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (LinearFunction):\n\t- must have a float attribute 'b'.");
    }

    SECTION("Poly5Function Ok") {
        doc = YAML::Load("Foo.Bar: { a: 1, b: 2, c: 3, d: 4, e: 5, f: 6 }");
        Result<Poly5Function> result = settings.readPoly5FunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(0.5) == Approx(10.03125));
    }

    SECTION("Poly5Function Bad") {
        doc = YAML::Load("Foo.Bar: { a: 1, c: 3, d: 4, e: 5 }");
        Result<Poly5Function> result = settings.readPoly5FunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (Poly5Function):\n\t- must have a float attribute 'b'.\n\t- must have a float attribute 'f'.");
    }

    SECTION("AnyBitSetFunction Ok") {
        doc = YAML::Load("Foo.Bar: { anyBitMask: 1 }");
        Result<AnyBitSetFunction> result = settings.readAnyBitSetFunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(1) == true);
    }

    SECTION("AnyBitSetFunction Bad") {
        doc = YAML::Load("Foo.Bar: { anyBitMask: a }");
        Result<AnyBitSetFunction> result = settings.readAnyBitSetFunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (AnyBitSetFunction):\n\t- must have an int attribute 'anyBitMask'.");
    }

    SECTION("AllBitSetFunction Ok") {
        doc = YAML::Load("Foo.Bar: { allBitMask: 15 }");
        Result<AllBitSetFunction> result = settings.readAllBitSetFunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(3) == false);
    }

    SECTION("AllBitSetFunction Bad") {
        doc = YAML::Load("Foo.Bar: { }");
        Result<AllBitSetFunction> result = settings.readAllBitSetFunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (AllBitSetFunction):\n\t- must have an int attribute 'allBitMask'.");
    }

    SECTION("AnyBitNotSetFunction Ok") {
        doc = YAML::Load("Foo.Bar: { anyBitNotMask: 3 }");
        Result<AnyBitNotSetFunction> result = settings.readAnyBitNotSetFunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(1) == true);
    }

    SECTION("AnyBitNotSetFunction Bad") {
        doc = YAML::Load("Foo.Bar: { anyBitNotMask: z }");
        Result<AnyBitNotSetFunction> result = settings.readAnyBitNotSetFunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (AnyBitNotSetFunction):\n\t- must have an int attribute 'anyBitNotMask'.");
    }

    SECTION("AllBitNotSetFunction Ok") {
        doc = YAML::Load("Foo.Bar: { allBitNotMask: 15 }");
        Result<AllBitNotSetFunction> result = settings.readAllBitNotSetFunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(16) == true);
    }

    SECTION("AllBitNotSetFunction Bad") {
        doc = YAML::Load("Foo.Bar: { }");
        Result<AllBitNotSetFunction> result = settings.readAllBitNotSetFunctionSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (AllBitNotSetFunction):\n\t- must have an int attribute 'allBitNotMask'.");
    }

    SECTION("NotEqualLimit Ok") {
        doc = YAML::Load("Foo.Bar: { notEqualTo: 1, limit: warning }");
        Result<NotEqualLimit> result = settings.readNotEqualLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(0) == Limits::WARNING);
    }

    SECTION("NotEqualLimit Bad") {
        doc = YAML::Load("Foo.Bar: { notEqualTo: 1 }");
        Result<NotEqualLimit> result = settings.readNotEqualLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (NotEqualLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("EqualLimit Ok") {
        doc = YAML::Load("Foo.Bar: { equalTo: 1, limit: warning }");
        Result<EqualLimit> result = settings.readEqualLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(1) == Limits::WARNING);
    }

    SECTION("EqualLimit Bad") {
                doc = YAML::Load("Foo.Bar: { equalTo: 1 }");
        Result<EqualLimit> result = settings.readEqualLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (EqualLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("LessThanLimit Ok") {
        doc = YAML::Load("Foo.Bar: { lessThan: 1, limit: warning }");
        Result<LessThanLimit> result = settings.readLessThanLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(0) == Limits::WARNING);
    }

    SECTION("LessThanLimit Bad") {
        doc = YAML::Load("Foo.Bar: { lessThan: 1 }");
        Result<LessThanLimit> result = settings.readLessThanLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (LessThanLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("LessThanEqualLimit Ok") {
        doc = YAML::Load("Foo.Bar: { lessThanEqualTo: 1, limit: warning }");
        Result<LessThanEqualLimit> result = settings.readLessThanEqualLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(1) == Limits::WARNING);
    }

    SECTION("LessThanEqualLimit Bad") {
        doc = YAML::Load("Foo.Bar: { lessThanEqualTo: 1 }");
        Result<LessThanEqualLimit> result = settings.readLessThanEqualLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (LessThanEqualLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("GreaterThanLimit Ok") {
        doc = YAML::Load("Foo.Bar: { greaterThan: 1, limit: warning }");
        Result<GreaterThanLimit> result = settings.readGreaterThanLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(2) == Limits::WARNING);
    }

    SECTION("GreaterThanLimit Bad") {
        doc = YAML::Load("Foo.Bar: { greaterThan: 1 }");
        Result<GreaterThanLimit> result = settings.readGreaterThanLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (GreaterThanLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("GreaterThanEqualLimit Ok") {
        doc = YAML::Load("Foo.Bar: { greaterThanEqualTo: 1, limit: warning }");
        Result<GreaterThanEqualLimit> result = settings.readGreaterThanEqualLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(1) == Limits::WARNING);
    }

    SECTION("GreaterThanEqualLimit Bad") {
        doc = YAML::Load("Foo.Bar: { greaterThanEqualTo: 1 }");
        Result<GreaterThanEqualLimit> result = settings.readGreaterThanEqualLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (GreaterThanEqualLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("NotInRangeLimit Ok") {
        doc = YAML::Load("Foo.Bar: { minValue: 1, maxValue: 2, limit: warning }");
        Result<NotInRangeLimit> result = settings.readNotInRangeLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(0) == Limits::WARNING);
    }

    SECTION("NotInRangeLimit Bad") {
        doc = YAML::Load("Foo.Bar: { minValue: 1, maxValue: 2 }");
        Result<NotInRangeLimit> result = settings.readNotInRangeLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (NotInRangeLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("InRangeLimit Ok") {
        doc = YAML::Load("Foo.Bar: { minValue: 1, maxValue: 2, limit: warning }");
        Result<InRangeLimit> result = settings.readInRangeLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(1.5) == Limits::WARNING);
    }

    SECTION("InRangeLimit Bad") {
        doc = YAML::Load("Foo.Bar: { minValue: 1, maxValue: 2 }");
        Result<InRangeLimit> result = settings.readInRangeLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (InRangeLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("NotInToleranceLimit Ok") {
        doc = YAML::Load("Foo.Bar: { target: 1.5, tolerance: 0.5, limit: warning }");
        Result<NotInToleranceLimit> result = settings.readNotInToleranceLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(0) == Limits::WARNING);
    }

    SECTION("NotInToleranceLimit Bad") {
        doc = YAML::Load("Foo.Bar: { target: 1.5, tolerance: 0.5 }");
        Result<NotInToleranceLimit> result = settings.readNotInToleranceLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (NotInToleranceLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("InToleranceLimit Ok") {
        doc = YAML::Load("Foo.Bar: { target: 1.5, tolerance: 0.5, limit: warning }");
        Result<InToleranceLimit> result = settings.readInToleranceLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(1.5) == Limits::WARNING);
    }

    SECTION("InToleranceLimit Bad") {
        doc = YAML::Load("Foo.Bar: { target: 1.5, tolerance: 0.5 }");
        Result<InToleranceLimit> result = settings.readInToleranceLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (InToleranceLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("AnyBitSetLimit Ok") {
        doc = YAML::Load("Foo.Bar: { anyBitMask: 1, limit: warning }");
        Result<AnyBitSetLimit> result = settings.readAnyBitSetLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(1) == Limits::WARNING);
    }

    SECTION("AnyBitSetLimit Bad") {
        doc = YAML::Load("Foo.Bar: { anyBitMask: 1, limit: 0.5 }");
        Result<AnyBitSetLimit> result = settings.readAnyBitSetLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (AnyBitSetLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("AllBitSetLimit Ok") {
        doc = YAML::Load("Foo.Bar: { allBitMask: 3, limit: fault }");
        Result<AllBitSetLimit> result = settings.readAllBitSetLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(3) == Limits::FAULT);
    }

    SECTION("AllBitSetLimit Bad") {
        doc = YAML::Load("Foo.Bar: { allBitMask: 1, limit: 0.5 }");
        Result<AllBitSetLimit> result = settings.readAllBitSetLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (AllBitSetLimit):\n\t- must have a limit attribute 'limit'.");
    }
    
    SECTION("AnyBitNotSetLimit Ok") {
        doc = YAML::Load("Foo.Bar: { anyBitNotMask: 1, limit: warning }");
        Result<AnyBitNotSetLimit> result = settings.readAnyBitNotSetLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(0) == Limits::WARNING);
    }

    SECTION("AnyBitNotSetLimit Bad") {
        doc = YAML::Load("Foo.Bar: { anyBitNotMask: 1, limit: 0.5 }");
        Result<AnyBitNotSetLimit> result = settings.readAnyBitNotSetLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (AnyBitNotSetLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("AllBitNotSetLimit Ok") {
        doc = YAML::Load("Foo.Bar: { allBitNotMask: 1, limit: warning }");
        Result<AllBitNotSetLimit> result = settings.readAllBitNotSetLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(0) == Limits::WARNING);
    }

    SECTION("AllBitNotSetLimit Bad") {
        doc = YAML::Load("Foo.Bar: { allBitNotMask: 1, limit: 0.5 }");
        Result<AllBitNotSetLimit> result = settings.readAllBitNotSetLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (AllBitNotSetLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("ContinuousTimedLimit Ok") {
        doc = YAML::Load("Foo.Bar: { threshold: 1, limit: warning }");
        Result<ContinuousTimedLimit> result = settings.readContinuousTimedLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(Limits::FAULT) == Limits::WARNING);
    }

    SECTION("ContinuousTimedLimit Bad") {
        doc = YAML::Load("Foo.Bar: { threshold: 1, limit: 0.5 }");
        Result<ContinuousTimedLimit> result = settings.readContinuousTimedLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (ContinuousTimedLimit):\n\t- must have a limit attribute 'limit'.");
    }

    SECTION("TimedLimit Ok") {
        doc = YAML::Load("Foo.Bar: { duration: 3, threshold: 1, limit: warning }");
        Result<TimedLimit> result = settings.readTimedLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value.evaluate(Limits::FAULT) == Limits::WARNING);
    }

    SECTION("TimedLimit Bad") {
        doc = YAML::Load("Foo.Bar: { duration: 3, threshold: 1, limit: 0.5 }");
        Result<TimedLimit> result = settings.readTimedLimitSetting(doc, "Foo.Bar");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "Setting 'Foo.Bar' (TimedLimit):\n\t- must have a limit attribute 'limit'.");
    }
}

 TEST_CASE("Settings Read Attribute", "[Settings]") {
    Settings settings = Settings();
    YAML::Node doc;

    SECTION("Bool is true") {
        doc = YAML::Load("value: true");
        Result<bool> result = settings.readBoolAttribute(doc, "value");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == true);
    }

    SECTION("Bool is false") {
        doc = YAML::Load("value: false");
        Result<bool> result = settings.readBoolAttribute(doc, "value");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == false);
    }

    SECTION("Bool is garbage") {
        doc = YAML::Load("value: maybe");
        Result<bool> result = settings.readBoolAttribute(doc, "value");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "\t- must have a boolean attribute 'value'.");
    }

    SECTION("Bool is undefined") {
        doc = YAML::Load("");
        Result<bool> result = settings.readBoolAttribute(doc, "value");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "\t- must have a boolean attribute 'value'.");
    }

    SECTION("Int is 3") {
        doc = YAML::Load("value: 3");
        Result<int64_t> result = settings.readIntAttribute(doc, "value");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == 3);
    }

    SECTION("Int is -2") {
        doc = YAML::Load("value: -2");
        Result<int64_t> result = settings.readIntAttribute(doc, "value");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == -2);
    }

    SECTION("Int is garbage") {
        doc = YAML::Load("value: foo");
        Result<int64_t> result = settings.readIntAttribute(doc, "value");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "\t- must have an int attribute 'value'.");
    }

    SECTION("Int is undefined") {
        doc = YAML::Load("");
        Result<int64_t> result = settings.readIntAttribute(doc, "value");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "\t- must have an int attribute 'value'.");
    }

    SECTION("Double is 3.12") {
        doc = YAML::Load("value: 3.12");
        Result<double> result = settings.readFloatAttribute(doc, "value");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == 3.12);
    }

    SECTION("Double is -2.21") {
        doc = YAML::Load("value: -2.21");
        Result<double> result = settings.readFloatAttribute(doc, "value");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == -2.21);
    }

    SECTION("Double is garbage") {
        doc = YAML::Load("value: foo");
        Result<double> result = settings.readFloatAttribute(doc, "value");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "\t- must have a float attribute 'value'.");
    }

    SECTION("Double is undefined") {
        doc = YAML::Load("");
        Result<double> result = settings.readFloatAttribute(doc, "value");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "\t- must have a float attribute 'value'.");
    }

    SECTION("String is Hello") {
        doc = YAML::Load("value: Hello");
        Result<std::string> result = settings.readStringAttribute(doc, "value");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == "Hello");
    }

    SECTION("String is World") {
        doc = YAML::Load("value: World");
        Result<std::string> result = settings.readStringAttribute(doc, "value");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == "World");
    }

    SECTION("String is undefined") {
        doc = YAML::Load("");
        Result<std::string> result = settings.readStringAttribute(doc, "value");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "\t- must have a string attribute 'value'.");
    }

    SECTION("Limit is Warning") {
        doc = YAML::Load("value: warning");
        Result<Limits> result = settings.readLimitAttribute(doc, "value");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == Limits::WARNING);
    }

    SECTION("Limit is Ok") {
        doc = YAML::Load("value: ok");
        Result<Limits> result = settings.readLimitAttribute(doc, "value");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == Limits::OK);
    }

    SECTION("Limit is garbage") {
        doc = YAML::Load("value: babababa");
        Result<Limits> result = settings.readLimitAttribute(doc, "value");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "\t- must have a limit attribute 'value'.");
    }

    SECTION("Limit is undefined") {
        doc = YAML::Load("");
        Result<Limits> result = settings.readLimitAttribute(doc, "value");

        REQUIRE(result.Valid == false);
        REQUIRE(result.Description == "\t- must have a limit attribute 'value'.");
    }
}

TEST_CASE("Settings Split String", "[Settings]") {
    Settings settings = Settings();
    std::string s = "AirSystem.AccumulatingAirPressureWarningTimedLimit.HARDPOINT_1";
    std::string delimiter = ".";
    std::vector<std::string> result = settings.split(s, delimiter);

    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == "AirSystem");
    REQUIRE(result[1] == "AccumulatingAirPressureWarningTimedLimit");
    REQUIRE(result[2] == "HARDPOINT_1");
}

TEST_CASE("Settings Parse Bool", "[Settings]") {
    Settings settings = Settings();

    SECTION("parse a value of true") {
        Result<bool> result = settings.parseBool("TrUE");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == true);
    }

    SECTION("parse a value of false") {
        Result<bool> result = settings.parseBool("FALse");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == false);
    }

    SECTION("error on empty input") {
        Result<bool> result = settings.parseBool("");

        REQUIRE(result.Valid == false);
    }

    SECTION("error on non-boolean input") {
        Result<bool> result = settings.parseBool("asd");

        REQUIRE(result.Valid == false);
    }
}

TEST_CASE("Settings Parse Int", "[Settings]") {
    Settings settings = Settings();

    SECTION("parse a positive number") {
        Result<int64_t> result = settings.parseInt("1234");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == 1234);
    }

    SECTION("parse a negative number") {
        Result<int64_t> result = settings.parseInt("-397464");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == -397464);
    }

    SECTION("parse a huge negative number") {
        Result<int64_t> result = settings.parseInt("-9000000876543210");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == -9000000876543210);
    }

    SECTION("parse a huge positive number") {
        Result<int64_t> result = settings.parseInt("+9010000876543210");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == 9010000876543210);
    }

    SECTION("parse a hex number") {
        Result<int64_t> result = settings.parseInt("0x10");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == 16);
    }

    SECTION("parse a hex number") {
        Result<int64_t> result = settings.parseInt("0x000A");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == 10);
    }

    SECTION("error on invalid character") {
        Result<int64_t> result = settings.parseInt("90100008a76543210");

        REQUIRE(result.Valid == false);
    }

    SECTION("error on empty input") {
        Result<int64_t> result = settings.parseInt("");

        REQUIRE(result.Valid == false);
    }
}

TEST_CASE("Settings Parse Float", "[Settings]") {
    Settings settings = Settings();

    SECTION("parse a positive number") {
        Result<double> result = settings.parseFloat("1234.1234");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == 1234.1234);
    }

    SECTION("parse a negative number") {
        Result<double> result = settings.parseFloat("-397464.97");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == -397464.97);
    }

    SECTION("parse a huge negative number") {
        Result<double> result = settings.parseFloat("-9000000876543210");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == -9000000876543210.0);
    }

    SECTION("parse a huge positive number") {
        Result<double> result = settings.parseFloat("+9010000876543210");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == 9010000876543210.0);
    }

    SECTION("parse lots of decimal places") {
        Result<double> result = settings.parseFloat("0.123456789");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == 0.123456789);
    }

    SECTION("error on invalid character") {
        Result<double> result = settings.parseFloat("90100008a76543210");

        REQUIRE(result.Valid == false);
    }

    SECTION("error on multiple decimal characters") {
        Result<double> result = settings.parseFloat("90100008.76543210.");

        REQUIRE(result.Valid == false);
    }

    SECTION("error on empty input") {
        Result<double> result = settings.parseFloat("");

        REQUIRE(result.Valid == false);
    }
}

TEST_CASE("Settings Parse Limit", "[Settings]") {
    Settings settings = Settings();

    SECTION("parse an unknown") {
        Result<Limits> result = settings.parseLimit("unknown");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == Limits::UNKNOWN);
    }

    SECTION("parse an ok") {
        Result<Limits> result = settings.parseLimit("ok");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == Limits::OK);
    }

    SECTION("parse an Warning") {
        Result<Limits> result = settings.parseLimit("Warning");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == Limits::WARNING);
    }

    SECTION("parse an FaUlT") {
        Result<Limits> result = settings.parseLimit("FaUlT");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == Limits::FAULT);
    }

    SECTION("parse an BYPASSED_OK") {
        Result<Limits> result = settings.parseLimit("BYPASSED_OK");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == Limits::BYPASSED_OK);
    }

    SECTION("parse an ByPaSsed_warning") {
        Result<Limits> result = settings.parseLimit("ByPaSsed_warning");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == Limits::BYPASSED_WARNING);
    }

    SECTION("parse an bypassed_fault") {
        Result<Limits> result = settings.parseLimit("bypassed_fault");

        REQUIRE(result.Valid == true);
        REQUIRE(result.Value == Limits::BYPASSED_FAULT);
    }

    SECTION("error on garbage") {
        Result<Limits> result = settings.parseLimit("90100008.76543210.");

        REQUIRE(result.Valid == false);
    }

    SECTION("error on empty input") {
        Result<Limits> result = settings.parseLimit("");

        REQUIRE(result.Valid == false);
    }
}
