/*
 * This file is part of LSST M1M3 thermal system package.
 *
 * Developed for the LSST Data Management System.
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
 */

#include <rapidcsv.h>
#include <spdlog/spdlog.h>

#include <cRIO/Settings/Path.h>

#include "Settings/AirNozzles.h"
#include "TSPublisher.h"

using namespace LSST::M1M3::TS::Settings;

const int NOZZLE_NUM = 275;

AirNozzles::AirNozzles(token) {}

void AirNozzles::load(const char *filename) {
    auto full_path = cRIO::Settings::Path::getFilePath("v1/tables/AirNozzles.csv");
    SPDLOG_INFO("Loading AirNozzles table from {}", full_path);
    try {
        rapidcsv::Document table(full_path, rapidcsv::LabelParams(), rapidcsv::SeparatorParams(),
                                 rapidcsv::ConverterParams(), rapidcsv::LineReaderParams(true, '#'));
        for (int index = 0; index < NOZZLE_NUM; index++) {
            nozzlesA[index] = -1;
            nozzlesB[index] = -1;
            nozzlesC[index] = -1;
            nozzlesD[index] = -1;
            nozzlesE[index] = -1;
            nozzlesF[index] = -1;
        }

        for (size_t row = 0; row < table.GetRowCount(); row++) {
            std::string label, type_str;
            try {
                label = table.GetCell<std::string>(0, row);
                type_str = table.GetCell<std::string>(1, row);

                // trim..
                type_str.erase(0, type_str.find_first_not_of(" \t"));
                type_str.erase(type_str.find_last_not_of(" \t") + 1);
                int type = 0;
                if (type_str == "SUPER_SHORT") {
                    type = MTM1M3TS::MTM1M3TS_shared_AirNozzle_SuperShort;
                } else if (type_str == "BLOCKED") {
                    type = MTM1M3TS::MTM1M3TS_shared_AirNozzle_Blocked;
                } else if (type_str == "OFFSET") {
                    type = MTM1M3TS::MTM1M3TS_shared_AirNozzle_Offset;
                } else if (type_str == "INSTALLED") {
                    type = MTM1M3TS::MTM1M3TS_shared_AirNozzle_Installed;
                } else if (type_str == "COVERED") {
                    type = MTM1M3TS::MTM1M3TS_shared_AirNozzle_Covered;
                } else {
                    throw std::runtime_error(
                            fmt::format("Unknown nozzle type for label {}: '{}'", label, type_str));
                };
                int index = std::stoi(label.substr(1)) - 1;
                if (index < 0 || index >= NOZZLE_NUM) {
                    throw std::runtime_error(
                            fmt::format("Invalid index in label {} on row {} should be in 1-{}.", label, row,
                                        NOZZLE_NUM));
                }
                switch (label[0]) {
                    case 'A':
                        nozzlesA[index] = type;
                        break;
                    case 'B':
                        nozzlesB[index] = type;
                        break;
                    case 'C':
                        nozzlesC[index] = type;
                        break;
                    case 'D':
                        nozzlesD[index] = type;
                        break;
                    case 'E':
                        nozzlesE[index] = type;
                        break;
                    case 'F':
                        nozzlesF[index] = type;
                        break;
                    default:
                        throw std::runtime_error(fmt::format(
                                "Mis-formatted nozzle label - expected [A-F][1-{}], find on row {} '{}'",
                                NOZZLE_NUM, row, label));
                }
            } catch (std::exception &ex) {
                throw std::runtime_error(fmt::format("Cannot read AirNozzles table - in {}:{} ({}, {}) - {}",
                                                     full_path, row, label, type_str, ex.what()));
            }
        }
    } catch (std::ios_base::failure &er) {
        throw std::runtime_error(
                fmt::format("Cannot read AirNozzles table from {}: {}", full_path, er.what()));
    }

    // check data
    for (int index = 0; index < NOZZLE_NUM; index++) {
#define CHECK(SECTOR)                                                                                        \
    if (nozzles##SECTOR[index] == -1) {                                                                      \
        throw std::runtime_error(                                                                            \
                fmt::format("File {} doesn't contain data for nozzle " #SECTOR "{}", full_path, index + 1)); \
    }
        CHECK(A);
        CHECK(B);
        CHECK(C);
        CHECK(D);
        CHECK(E);
        CHECK(F);
    }
}

void AirNozzles::send() {
    salReturn ret = TSPublisher::SAL()->putSample_logevent_airNozzles(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot publis AirNozzles: {}", ret);
    }
}
