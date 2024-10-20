/*
 * Simulated flow meter device.
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

#include <spdlog/spdlog.h>

#include <cRIO/MPU.h>

#include "SimulatedFlowMeter.h"

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;

SimulatedFlowMeter::SimulatedFlowMeter() {
    _signal.value = 0;
    _flow_rate.value = 1.1;
    _flowmeter_net_totalizer.value = 100.1;
}

void SimulatedFlowMeter::generate_response(const unsigned char* buf, size_t len) {
    Modbus::Parser parser(std::vector<uint8_t>(buf, buf + len));
    _response.push_back(parser.address());
    switch (parser.func()) {
        case MPU::READ_HOLDING_REGISTERS: {
            uint16_t reg = parser.read<uint16_t>();
            uint16_t reg_len = parser.read<uint16_t>() * 2;
            _response.push_back(parser.func());
            _response.push_back(reg_len);
            for (size_t i = 0; i < reg_len; i += 2, reg++) {
                // simulates various registers
                switch (reg) {
                    case 1600:
                        _flow_rate.value += 3.3;
                    case 1601:
                        _response.push_back(_flow_rate.bytes[3 - 2 * (reg - 1600)]);
                        _response.push_back(_flow_rate.bytes[2 - 2 * (reg - 1600)]);
                        break;
                    case 2600:
                        _flowmeter_net_totalizer.value += 11.1;
                    case 2601:
                    case 2602:
                    case 2603:
                        _response.push_back(_flowmeter_net_totalizer.bytes[7 - 2 * (reg - 2600)]);
                        _response.push_back(_flowmeter_net_totalizer.bytes[6 - 2 * (reg - 2600)]);
                        break;
                    case 2604:
                    case 2605:
                    case 2606:
                    case 2607:
                        _response.push_back(_flowmeter_net_totalizer.bytes[7 - 2 * (reg - 2604)]);
                        _response.push_back(_flowmeter_net_totalizer.bytes[6 - 2 * (reg - 2604)]);
                        break;
                    case 2608:
                    case 2609:
                    case 2610:
                    case 2611:
                        _response.push_back(_flowmeter_net_totalizer.bytes[7 - 2 * (reg - 2608)]);
                        _response.push_back(_flowmeter_net_totalizer.bytes[6 - 2 * (reg - 2608)]);
                        break;
                    case 5500:
                        _response.push_back(_signal.bytes[1]);
                        _response.push_back(_signal.bytes[0]);
                        _signal.value += 1;
                        break;
                    default:
                        _response.push_back(i);
                        _response.push_back(i + 1);
                }
            }
            break;
        }
        case MPU::PRESET_HOLDING_REGISTER: {
            uint16_t reg = parser.read<uint16_t>();
            uint16_t reg_len = parser.read<uint16_t>();

            _response.push_back(parser.func());
            _response.write(reg);
            _response.write(reg_len);

            break;
        }
        default:
            throw std::runtime_error(fmt::format("Response for function {} not implemented", parser.func()));
    }
    _response.writeCRC();
}
