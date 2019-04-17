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

#ifndef SETTINGS_SETTINGS_H_
#define SETTINGS_SETTINGS_H_

#include <string>
#include <vector>

#include "../Domain/Domain.h"
#include "../Domain/Functions.h"
#include "../Domain/Limits.h"
#include "../Domain/Result.h"
#include "yaml-cpp/yaml.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

class Settings {
private:
    std::string basePath;
    MainSystemSettings mainSystemSettings;
    InterlockSystemSettings interlockSystemSettings;

public:
    Settings();

    std::string getBasePath();
    MainSystemSettings& getMainSystemSettings() { return this->mainSystemSettings; }
    InterlockSystemSettings& getInterlockSystemSettings() { return this->interlockSystemSettings; }

    Result<std::string> getSettingPath(std::string tag);

    Result<bool> loadFromString(std::string yaml, bool required = true);
    Result<bool> loadFromFile(std::string filePath, bool required = true);
    Result<bool> loadFromDocument(YAML::Node node, bool required);

    Result<LinearFunction> readLinearFunctionSetting(YAML::Node node, std::string id, bool required = true);
    Result<Poly5Function> readPoly5FunctionSetting(YAML::Node node, std::string id, bool required = true);
    Result<AnyBitSetFunction> readAnyBitSetFunctionSetting(YAML::Node node, std::string id, bool required = true);
    Result<AllBitSetFunction> readAllBitSetFunctionSetting(YAML::Node node, std::string id, bool required = true);
    Result<AnyBitNotSetFunction> readAnyBitNotSetFunctionSetting(YAML::Node node, std::string id, bool required = true);
    Result<AllBitNotSetFunction> readAllBitNotSetFunctionSetting(YAML::Node node, std::string id, bool required = true);
    Result<NotEqualLimit> readNotEqualLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<EqualLimit> readEqualLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<LessThanLimit> readLessThanLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<LessThanEqualLimit> readLessThanEqualLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<GreaterThanLimit> readGreaterThanLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<GreaterThanEqualLimit> readGreaterThanEqualLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<NotInRangeLimit> readNotInRangeLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<InRangeLimit> readInRangeLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<NotInToleranceLimit> readNotInToleranceLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<InToleranceLimit> readInToleranceLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<AnyBitSetLimit> readAnyBitSetLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<AllBitSetLimit> readAllBitSetLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<AnyBitNotSetLimit> readAnyBitNotSetLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<AllBitNotSetLimit> readAllBitNotSetLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<ContinuousTimedLimit> readContinuousTimedLimitSetting(YAML::Node node, std::string id, bool required = true);
    Result<TimedLimit> readTimedLimitSetting(YAML::Node node, std::string id, bool required = true);

    Result<bool> readBoolAttribute(YAML::Node node, std::string name);
    Result<int64_t> readIntAttribute(YAML::Node node, std::string name);
    Result<double> readFloatAttribute(YAML::Node node, std::string name);
    Result<std::string> readStringAttribute(YAML::Node node, std::string name);
    Result<Limits> readLimitAttribute(YAML::Node node, std::string name);

    std::vector<std::string> split(std::string input, std::string delimiter);

    Result<bool> parseBool(std::string input);
    Result<int64_t> parseInt(std::string input);
    Result<double> parseFloat(std::string input);
    Result<Limits> parseLimit(std::string input);

    void setBasePath();
    bool fileExists(std::string filePath);
    std::string intToString(int32_t i);
};

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */

#endif /* SETTINGS_SETTINGS_H_ */
