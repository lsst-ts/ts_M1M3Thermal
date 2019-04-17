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

#include "Settings.h"

#include <fstream>
#include <sstream>

#include "../Utility/whereami.h"

/******************************************************************************
 * Note:
 * This macro is only used to reduce the number of lines of code this file
 * would be (and therefore reduce copy paste errors).
 * When calling this MACRO make sure the following preconditions are met:
 * 1. pugi::xml_document doc exists
 * 2. this->read*Setting(YAML::Node, bool) exists (where * is resultType)
 * 3. Result<*> result exists (where * is resultType)
 *****************************************************************************/
#define READ_FIELD(resultType, model, field) { \
    { \
        YAML::Node node = doc.select_node("//Setting[@id='" #model "." #field "']").node(); \
        Result<resultType> nodeResult = this->read ## resultType ## Setting(node, #model "." #field, required); \
        if (nodeResult.Valid) { \
            result.Value.field = nodeResult.Value; \
        } \
        else { \
            result.Valid = false; \
            result.Description += "\n" + nodeResult.Description; \
        } \
    } \
}

/******************************************************************************
 * Note:
 * This macro is only used to reduce the number of lines of code this file
 * would be (and therefore reduce copy paste errors).
 * When calling this MACRO make sure the following preconditions are met:
 * 1. pugi::xml_document doc exists
 * 2. this->read*Setting(YAML::Node, bool) exists (where * is resultType)
 * 3. Result<*> result exists (where * is resultType)
 * 4. index is a valid index into the collection
 *****************************************************************************/
#define READ_INDEXED_FIELD(resultType, model, field, index) { \
    { \
        YAML::Node node = doc.select_node("//Setting[@id='" #model "." #field "." #index "']").node(); \
        Result<resultType> nodeResult = this->read ## resultType ## Setting(node, #model "." #field "." #index, required); \
        if (nodeResult.Valid) { \
            result.Value.field[index] = nodeResult.Value; \
        } \
        else { \
            result.Valid = false; \
            result.Description += "\n" + nodeResult.Description; \
        } \
    } \
}

/******************************************************************************
 * Note:
 * This macro is only used to reduce the number of lines of code this file
 * would be (and therefore reduce copy paste errors).
 * When calling this MACRO make sure the following preconditions are met:
 * 1. pugi::xml_document doc exists
 * 2. this->read*(pugi::xml_doc, bool) exists (where * is resultType)
 * 3. Result<*> result exists (where * is resultType)
 * 4. Class contains a * this->field (where * is resultType)
 *****************************************************************************/
#define READ_SETTING(resultType, field) { \
    { \
        Result<resultType> settingResult = this->read ## resultType(doc, required); \
        if (settingResult.Valid) { \
            this->field = settingResult.Value; \
        } \
        else { \
            result.Valid = false; \
            result.Description += "\n" + settingResult.Description; \
        } \
    } \
}

namespace LSST {
namespace TS {
namespace MTM1M3TS {

Settings::Settings() { 
    this->setBasePath();
}

std::string Settings::getBasePath() { return this->basePath; }

Result<std::string> Settings::getSettingPath(std::string tag) {
    Result<std::string> result;

    std::vector<std::string> tokens = this->split(tag, ",");
    result.Value = this->getBasePath() + "config/";
    if (tokens.size() == 2) {
        Result<int64_t> version = this->parseInt(tokens[1]);
        if (version.Valid) {
            result.Value = result.Value + tokens[0] + "-" + tokens[1] + ".yaml";
            if (!this->fileExists(result.Value)) {
                result.Valid = false;
                result.Description = "No setting file with name '" + tokens[0] + "' and version '" + tokens[1] + "' exist.";
            }
        }
        else {
            result.Valid = false;
            result.Description = version.Description;
        }
    }
    else if (tokens.size() == 1) {
        int32_t i = 1;
        std::string testPath = result.Value + tokens[0] + "-" + this->intToString(i) + ".yaml";
        while(this->fileExists(testPath)) {
            ++i;
            testPath = result.Value + tokens[0] + "-" + this->intToString(i) + ".yaml";
        }
        if (i != 1) {
            --i;
            result.Value = result.Value + tokens[0] + "-" + this->intToString(i) + ".yaml";
        }
        else {
            result.Valid = false;
            result.Description = "No setting files with '" + tokens[0] + "' name exist.";
        }
    }
    else {
        result.Valid = false;
        result.Description = "Setting tag '" + tag + "' is not a valid tag. Valid tags are 'name,version' (ex: default,1).";
    }
    return result;
}

Result<bool> Settings::loadFromString(std::string yaml, bool required) {
    YAML::Node node = YAML::Load(yaml);
    return this->loadFromDocument(node, required);
}

Result<bool> Settings::loadFromFile(std::string filePath, bool required) {
    if (this->fileExists(filePath.c_str())) {
        YAML::Node node = YAML::LoadFile(filePath);
        return this->loadFromDocument(node, required);
    }
    return Result<bool>(false, false, "File '" + filePath + "' does not exist.");
}

Result<bool> Settings::loadFromDocument(YAML::Node node, bool required) {
    Result<bool> result;

    return result;
}

Result<LinearFunction> Settings::readLinearFunctionSetting(YAML::Node node, std::string id, bool required) {
    Result<LinearFunction> result;

    YAML::Node settingNode = node[id];
    Result<double> m = this->readFloatAttribute(settingNode, "m");
    Result<double> b = this->readFloatAttribute(settingNode, "b");

    if (m.Valid && b.Valid) {
        result.Value = LinearFunction(m.Value, b.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (LinearFunction):";
        if (!m.Valid) {
            result.Description += "\n" + m.Description;
        }
        if (!b.Valid) {
            result.Description += "\n" + b.Description;
        }
    }
    
    return result;
}

Result<Poly5Function> Settings::readPoly5FunctionSetting(YAML::Node node, std::string id, bool required) {
    Result<Poly5Function> result;

    YAML::Node settingNode = node[id];
    Result<double> a = this->readFloatAttribute(settingNode, "a");
    Result<double> b = this->readFloatAttribute(settingNode, "b");
    Result<double> c = this->readFloatAttribute(settingNode, "c");
    Result<double> d = this->readFloatAttribute(settingNode, "d");
    Result<double> e = this->readFloatAttribute(settingNode, "e");
    Result<double> f = this->readFloatAttribute(settingNode, "f");

    if (a.Valid && b.Valid && c.Valid && d.Valid && e.Valid && f.Valid) {
        result.Value = Poly5Function(a.Value, b.Value, c.Value, d.Value, e.Value, f.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (Poly5Function):";
        if (!a.Valid) {
            result.Description += "\n" + a.Description;
        }
        if (!b.Valid) {
            result.Description += "\n" + b.Description;
        }
        if (!c.Valid) {
            result.Description += "\n" + c.Description;
        }
        if (!d.Valid) {
            result.Description += "\n" + d.Description;
        }
        if (!e.Valid) {
            result.Description += "\n" + e.Description;
        }
        if (!f.Valid) {
            result.Description += "\n" + f.Description;
        }
    }

    return result;
}

Result<AnyBitSetFunction> Settings::readAnyBitSetFunctionSetting(YAML::Node node, std::string id, bool required) {
    Result<AnyBitSetFunction> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> mask = this->readIntAttribute(settingNode, "anyBitMask");

    if (mask.Valid) {
        result.Value = AnyBitSetFunction(mask.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (AnyBitSetFunction):";
        if (!mask.Valid) {
            result.Description += "\n" + mask.Description;
        }
    }

    return result;
}

Result<AllBitSetFunction> Settings::readAllBitSetFunctionSetting(YAML::Node node, std::string id, bool required) {
    Result<AllBitSetFunction> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> mask = this->readIntAttribute(settingNode, "allBitMask");

    if (mask.Valid) {
        result.Value = AllBitSetFunction(mask.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (AllBitSetFunction):";
        if (!mask.Valid) {
            result.Description += "\n" + mask.Description;
        }
    }

    return result;
}

Result<AnyBitNotSetFunction> Settings::readAnyBitNotSetFunctionSetting(YAML::Node node, std::string id, bool required) {
    Result<AnyBitNotSetFunction> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> mask = this->readIntAttribute(settingNode, "anyBitNotMask");

    if (mask.Valid) {
        result.Value = AnyBitNotSetFunction(mask.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (AnyBitNotSetFunction):";
        if (!mask.Valid) {
            result.Description += "\n" + mask.Description;
        }
    }

    return result;
}

Result<AllBitNotSetFunction> Settings::readAllBitNotSetFunctionSetting(YAML::Node node, std::string id, bool required) {
    Result<AllBitNotSetFunction> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> mask = this->readIntAttribute(settingNode, "allBitNotMask");

    if (mask.Valid) {
        result.Value = AllBitNotSetFunction(mask.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (AllBitNotSetFunction):";
        if (!mask.Valid) {
            result.Description += "\n" + mask.Description;
        }
    }

    return result;
}

Result<NotEqualLimit> Settings::readNotEqualLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<NotEqualLimit> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> threshold = this->readIntAttribute(settingNode, "notEqualTo");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (threshold.Valid && limit.Valid) {
        result.Value = NotEqualLimit(threshold.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (NotEqualLimit):";
        if (!threshold.Valid) {
            result.Description += "\n" + threshold.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<EqualLimit> Settings::readEqualLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<EqualLimit> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> threshold = this->readIntAttribute(settingNode, "equalTo");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (threshold.Valid && limit.Valid) {
        result.Value = EqualLimit(threshold.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (EqualLimit):";
        if (!threshold.Valid) {
            result.Description += "\n" + threshold.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<LessThanLimit> Settings::readLessThanLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<LessThanLimit> result;

    YAML::Node settingNode = node[id];
    Result<double> threshold = this->readFloatAttribute(settingNode, "lessThan");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (threshold.Valid && limit.Valid) {
        result.Value = LessThanLimit(threshold.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (LessThanLimit):";
        if (!threshold.Valid) {
            result.Description += "\n" + threshold.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<LessThanEqualLimit> Settings::readLessThanEqualLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<LessThanEqualLimit> result;

    YAML::Node settingNode = node[id];
    Result<double> threshold = this->readFloatAttribute(settingNode, "lessThanEqualTo");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (threshold.Valid && limit.Valid) {
        result.Value = LessThanEqualLimit(threshold.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (LessThanEqualLimit):";
        if (!threshold.Valid) {
            result.Description += "\n" + threshold.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<GreaterThanLimit> Settings::readGreaterThanLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<GreaterThanLimit> result;

    YAML::Node settingNode = node[id];
    Result<double> threshold = this->readFloatAttribute(settingNode, "greaterThan");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (threshold.Valid && limit.Valid) {
        result.Value = GreaterThanLimit(threshold.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (GreaterThanLimit):";
        if (!threshold.Valid) {
            result.Description += "\n" + threshold.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<GreaterThanEqualLimit> Settings::readGreaterThanEqualLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<GreaterThanEqualLimit> result;

    YAML::Node settingNode = node[id];
    Result<double> threshold = this->readFloatAttribute(settingNode, "greaterThanEqualTo");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (threshold.Valid && limit.Valid) {
        result.Value = GreaterThanEqualLimit(threshold.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (GreaterThanEqualLimit):";
        if (!threshold.Valid) {
            result.Description += "\n" + threshold.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<NotInRangeLimit> Settings::readNotInRangeLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<NotInRangeLimit> result;

    YAML::Node settingNode = node[id];
    Result<double> minValue = this->readFloatAttribute(settingNode, "minValue");
    Result<double> maxValue = this->readFloatAttribute(settingNode, "maxValue");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (minValue.Valid && maxValue.Valid && limit.Valid) {
        result.Value = NotInRangeLimit(minValue.Value, maxValue.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (NotInRangeLimit):";
        if (!minValue.Valid) {
            result.Description += "\n" + minValue.Description;
        }
        if (!maxValue.Valid) {
            result.Description += "\n" + maxValue.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<InRangeLimit> Settings::readInRangeLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<InRangeLimit> result;

    YAML::Node settingNode = node[id];
    Result<double> minValue = this->readFloatAttribute(settingNode, "minValue");
    Result<double> maxValue = this->readFloatAttribute(settingNode, "maxValue");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (minValue.Valid && maxValue.Valid && limit.Valid) {
        result.Value = InRangeLimit(minValue.Value, maxValue.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (InRangeLimit):";
        if (!minValue.Valid) {
            result.Description += "\n" + minValue.Description;
        }
        if (!maxValue.Valid) {
            result.Description += "\n" + maxValue.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<NotInToleranceLimit> Settings::readNotInToleranceLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<NotInToleranceLimit> result;

    YAML::Node settingNode = node[id];
    Result<double> target = this->readFloatAttribute(settingNode, "target");
    Result<double> tolerance = this->readFloatAttribute(settingNode, "tolerance");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (target.Valid && tolerance.Valid && limit.Valid) {
        result.Value = NotInToleranceLimit(target.Value, tolerance.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (NotInToleranceLimit):";
        if (!target.Valid) {
            result.Description += "\n" + target.Description;
        }
        if (!tolerance.Valid) {
            result.Description += "\n" + tolerance.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<InToleranceLimit> Settings::readInToleranceLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<InToleranceLimit> result;

    YAML::Node settingNode = node[id];
    Result<double> target = this->readFloatAttribute(settingNode, "target");
    Result<double> tolerance = this->readFloatAttribute(settingNode, "tolerance");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (target.Valid && tolerance.Valid && limit.Valid) {
        result.Value = InToleranceLimit(target.Value, tolerance.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (InToleranceLimit):";
        if (!target.Valid) {
            result.Description += "\n" + target.Description;
        }
        if (!tolerance.Valid) {
            result.Description += "\n" + tolerance.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<AnyBitSetLimit> Settings::readAnyBitSetLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<AnyBitSetLimit> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> mask = this->readIntAttribute(settingNode, "anyBitMask");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (mask.Valid && limit.Valid) {
        result.Value = AnyBitSetLimit(mask.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (AnyBitSetLimit):";
        if (!mask.Valid) {
            result.Description += "\n" + mask.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<AllBitSetLimit> Settings::readAllBitSetLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<AllBitSetLimit> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> mask = this->readIntAttribute(settingNode, "allBitMask");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (mask.Valid && limit.Valid) {
        result.Value = AllBitSetLimit(mask.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (AllBitSetLimit):";
        if (!mask.Valid) {
            result.Description += "\n" + mask.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<AnyBitNotSetLimit> Settings::readAnyBitNotSetLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<AnyBitNotSetLimit> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> mask = this->readIntAttribute(settingNode, "anyBitNotMask");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (mask.Valid && limit.Valid) {
        result.Value = AnyBitNotSetLimit(mask.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (AnyBitNotSetLimit):";
        if (!mask.Valid) {
            result.Description += "\n" + mask.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<AllBitNotSetLimit> Settings::readAllBitNotSetLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<AllBitNotSetLimit> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> mask = this->readIntAttribute(settingNode, "allBitNotMask");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (mask.Valid && limit.Valid) {
        result.Value = AllBitNotSetLimit(mask.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (AllBitNotSetLimit):";
        if (!mask.Valid) {
            result.Description += "\n" + mask.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<ContinuousTimedLimit> Settings::readContinuousTimedLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<ContinuousTimedLimit> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> threshold = this->readIntAttribute(settingNode, "threshold");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (threshold.Valid && limit.Valid) {
        result.Value = ContinuousTimedLimit(threshold.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (ContinuousTimedLimit):";
        if (!threshold.Valid) {
            result.Description += "\n" + threshold.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<TimedLimit> Settings::readTimedLimitSetting(YAML::Node node, std::string id, bool required) {
    Result<TimedLimit> result;

    YAML::Node settingNode = node[id];
    Result<int64_t> duration = this->readIntAttribute(settingNode, "duration");
    Result<int64_t> threshold = this->readIntAttribute(settingNode, "threshold");
    Result<Limits> limit = this->readLimitAttribute(settingNode, "limit");

    if (duration.Valid && threshold.Valid && limit.Valid) {
        result.Value = TimedLimit(duration.Value, threshold.Value, limit.Value);
    }
    else if (!node.IsDefined() || required) {
        result.Valid = false;
        result.Description = "Setting '" + id + "' (TimedLimit):";
        if (!duration.Valid) {
            result.Description += "\n" + duration.Description;
        }
        if (!threshold.Valid) {
            result.Description += "\n" + threshold.Description;
        }
        if (!limit.Valid) {
            result.Description += "\n" + limit.Description;
        }
    }

    return result;
}

Result<bool> Settings::readBoolAttribute(YAML::Node node, std::string name) {
    Result<bool> result;

    YAML::Node inputNode = node[name];
    if (inputNode.IsDefined()) {
        std::string input = inputNode.as<std::string>();
        Result<bool> inputResult = this->parseBool(input);
    
        if (inputResult.Valid) {
            result.Value = inputResult.Value;
        }
        else {
            result.Valid = false;
            result.Description = "\t- must have a boolean attribute '" + name + "'.";
        }
    }
    else {
        result.Valid = false;
        result.Description = "\t- must have a boolean attribute '" + name + "'.";
    }

    return result;
}

Result<int64_t> Settings::readIntAttribute(YAML::Node node, std::string name) {
    Result<int64_t> result;

    YAML::Node inputNode = node[name];
    if (inputNode.IsDefined()) {
        std::string input = inputNode.as<std::string>();
        Result<int64_t> inputResult = this->parseInt(input);

        if (inputResult.Valid) {
            result.Value = inputResult.Value;
        }
        else {
            result.Valid = false;
            result.Description = "\t- must have an int attribute '" + name + "'.";
        }
    }
    else {
        result.Valid = false;
        result.Description = "\t- must have an int attribute '" + name + "'.";
    }

    return result;
}

Result<double> Settings::readFloatAttribute(YAML::Node node, std::string name) {
    Result<double> result;

    YAML::Node inputNode = node[name];
    if (inputNode.IsDefined()) {
        std::string input = inputNode.as<std::string>();
        Result<double> inputResult = this->parseFloat(input);

        if (inputResult.Valid) {
            result.Value = inputResult.Value;
        }
        else {
            result.Valid = false;
            result.Description = "\t- must have a float attribute '" + name + "'.";
        }
    }
    else {
        result.Valid = false;
        result.Description = "\t- must have a float attribute '" + name + "'.";
    }

    return result;
}

Result<std::string> Settings::readStringAttribute(YAML::Node node, std::string name) {
    Result<std::string> result;

    YAML::Node inputNode = node[name];
    if (inputNode.IsDefined()) {
        result.Value = inputNode.as<std::string>();
    }
    else {
        result.Valid = false;
        result.Description = "\t- must have a string attribute '" + name + "'.";
    }

    return result;
}

Result<Limits> Settings::readLimitAttribute(YAML::Node node, std::string name) {
    Result<Limits> result;
    
    YAML::Node inputNode = node[name];
    if (inputNode.IsDefined()) {
        std::string input = node[name].as<std::string>();
        Result<Limits> inputResult = this->parseLimit(input);

        if (inputResult.Valid) {
            result.Value = inputResult.Value;
        }
        else {
            result.Valid = false;
            result.Description = "\t- must have a limit attribute '" + name + "'.";
        }
    }
    else {
        result.Valid = false;
        result.Description = "\t- must have a limit attribute '" + name + "'.";
    }

    return result;
}

std::vector<std::string> Settings::split(std::string input, std::string delimiter) {
    std::vector<std::string> result;

    int previousDelimiterPos = -1;
    int delimiterPos = input.find(delimiter);
    while(delimiterPos != -1) {
        std::string token = input.substr(previousDelimiterPos + 1, delimiterPos - previousDelimiterPos - 1);
        result.push_back(token);
        previousDelimiterPos = delimiterPos;
        delimiterPos = input.find(delimiter, previousDelimiterPos + 1);
    }
    if((input.size() - previousDelimiterPos) > 1) {
        std::string token = input.substr(previousDelimiterPos + 1);
        result.push_back(token);
    }
    result.reserve(result.size());

    return result;
}

Result<bool> Settings::parseBool(std::string input) {
    Result<bool> result;

    if (input.size() > 0) {
        std::string lowerInput = std::string(input);
            
        for(uint32_t i = 0; i < lowerInput.size(); ++i) {
            lowerInput[i] = tolower(lowerInput[i]);
        }

        if (lowerInput == "true") {
            result.Value = true;
        }
        else if (lowerInput == "false") {
            result.Value = false;
        }
        else {
            result.Valid = false;
            result.Description = "Cannot parse '" + input + "' as boolean value. ";
        }
    }
    else {
        result.Valid = false;
        result.Description = "Cannot parse empty input as bool. ";
    }

    return result;
}

Result<int64_t> Settings::parseInt(std::string input) {
    Result<int64_t> result;

    if (input.size() > 0) {
        bool isHex = false;
        bool negative = false;
        uint32_t startIndex = 0;
        int64_t base = 10;
        if (input[0] == '0' && (input[1] == 'x' || input[1] == 'X')) {
            isHex = true;
            startIndex = 2;
            base = 16;
        }
        else if (input[0] == '+') {
            negative = false;
            startIndex = 1;
        }
        else if (input[0] == '-') {
            negative = true;
            startIndex = 1;
        }
        for(uint32_t i = startIndex; i < input.size() && result.Valid; ++i) {
            switch(input[i]) {
                case '0': result.Value = result.Value * base + 0; break;
                case '1': result.Value = result.Value * base + 1; break;
                case '2': result.Value = result.Value * base + 2; break;
                case '3': result.Value = result.Value * base + 3; break;
                case '4': result.Value = result.Value * base + 4; break;
                case '5': result.Value = result.Value * base + 5; break;
                case '6': result.Value = result.Value * base + 6; break;
                case '7': result.Value = result.Value * base + 7; break;
                case '8': result.Value = result.Value * base + 8; break;
                case '9': result.Value = result.Value * base + 9; break;
                default: 
                if (isHex) {
                    switch(input[i]) {
                        case 'a':
                        case 'A': result.Value = result.Value * base + 10; break;
                        case 'b':
                        case 'B': result.Value = result.Value * base + 11; break;
                        case 'c':
                        case 'C': result.Value = result.Value * base + 12; break;
                        case 'd':
                        case 'D': result.Value = result.Value * base + 13; break;
                        case 'e':
                        case 'E': result.Value = result.Value * base + 14; break;
                        case 'f':
                        case 'F': result.Value = result.Value * base + 15; break;
                        default:
                            result.Valid = false; 
                            result.Description = "Cannot parse input '" + input + "' as integer value. ";
                            break;
                    }
                }
                else {
                    result.Valid = false; 
                    result.Description = "Cannot parse input '" + input + "' as integer value. ";
                }
            }
        }
        if (negative) {
            result.Value = result.Value * -1;
        }
    }
    else {
        result.Valid = false;
        result.Description = "Cannot parse empty input as int. ";
    }

    return result;
}

Result<double> Settings::parseFloat(std::string input) {
    Result<double> result;

    if (input.size() > 0) {
        bool negative = false;
        uint32_t startIndex = 0;
        double divisor = 0;

        if (input[0] == '+') {
            negative = false;
            startIndex = 1;
        }
        else if (input[0] == '-') {
            negative = true;
            startIndex = 1;
        }
        for(uint32_t i = startIndex; i < input.size() && result.Valid; ++i) {
            if (divisor != 0) {
                divisor *= 10.0;
            }
            switch(input[i]) {
                case '.': 
                    if (divisor != 0) {
                        result.Valid = false;
                        result.Description = "Cannot parse input '" + input + "' as float value. ";
                    }
                    else {
                        divisor = 1.0;
                    }
                    break;
                case '0': result.Value = result.Value * 10.0 + 0.0; break;
                case '1': result.Value = result.Value * 10.0 + 1.0; break;
                case '2': result.Value = result.Value * 10.0 + 2.0; break;
                case '3': result.Value = result.Value * 10.0 + 3.0; break;
                case '4': result.Value = result.Value * 10.0 + 4.0; break;
                case '5': result.Value = result.Value * 10.0 + 5.0; break;
                case '6': result.Value = result.Value * 10.0 + 6.0; break;
                case '7': result.Value = result.Value * 10.0 + 7.0; break;
                case '8': result.Value = result.Value * 10.0 + 8.0; break;
                case '9': result.Value = result.Value * 10.0 + 9.0; break;
                default: 
                    result.Valid = false; 
                    result.Description = "Cannot parse input '" + input + "' as float value. ";
                    break;
            }
        }
        if (negative) {
            result.Value = result.Value * -1;
        }
        if (divisor != 0) {
            result.Value = result.Value / divisor;
        }
    }
    else {
        result.Valid = false;
        result.Description = "Cannot parse empty input as float. ";
    }

    return result;
}

Result<Limits> Settings::parseLimit(std::string input) {
    Result<Limits> result;

    if (input.size() > 0) {
        std::string lowerInput = std::string(input);
            
        for(uint32_t i = 0; i < lowerInput.size(); ++i) {
            lowerInput[i] = tolower(lowerInput[i]);
        }

        if (lowerInput == "unknown") {
            result.Value = Limits::UNKNOWN;
        }
        else if (lowerInput == "ok") {
            result.Value = Limits::OK;
        }
        else if (lowerInput == "warning") {
            result.Value = Limits::WARNING;
        }
        else if (lowerInput == "fault") {
            result.Value = Limits::FAULT;
        }
        else if (lowerInput == "bypassed_ok") {
            result.Value = Limits::BYPASSED_OK;
        }
        else if (lowerInput == "bypassed_warning") {
            result.Value = Limits::BYPASSED_WARNING;
        }
        else if (lowerInput == "bypassed_fault") {
            result.Value = Limits::BYPASSED_FAULT;
        }
        else {
            result.Valid = false;
            result.Description = "Cannot parse '" + input + "' as limit value. ";
        }
    }
    else {
        result.Valid = false;
        result.Description = "Cannot parse empty input as limit. ";
    }

    return result;
}

void Settings::setBasePath() {
    char buffer[1024];
    int32_t length = 0;
    wai_getExecutablePath(buffer, 1024, &length);
    std::string exePath = std::string(buffer);
    int32_t lastIndex = exePath.find_last_of('/');
    this->basePath = exePath.substr(0, lastIndex + 1);
}

bool Settings::fileExists(std::string filePath) {
    std::ifstream ifile(filePath.c_str());
    return (bool)ifile;
}

std::string Settings::intToString(int32_t i) {
    std::string output;
    std::stringstream ss;
    ss << i;
    output = ss.str();
    return output;
}

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */
