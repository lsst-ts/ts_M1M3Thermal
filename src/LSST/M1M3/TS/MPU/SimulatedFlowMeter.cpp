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

#include "FlowMeter.h"
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
    _response.vector.push_back(parser.address());
    switch (parser.func()) {
        case MPU::READ_HOLDING_REGISTERS: {
            uint16_t reg = parser.read<uint16_t>();
            uint16_t reg_len = parser.read<uint16_t>() * 2;
            _response.vector.push_back(parser.func());
            _response.vector.push_back(reg_len);
            for (size_t i = 0; i < reg_len; i += 2, reg++) {
                // simulates various registers
                switch (reg) {
                    case FlowMeter::FLOW_RATE:
                        _flow_rate.value += 3.3;
                    case FlowMeter::FLOW_RATE + 1:
                    case FlowMeter::FLOW_RATE + 2:
                    case FlowMeter::FLOW_RATE + 3:
                        _response.vector.push_back(_flow_rate.bytes[7 - 2 * (reg - FlowMeter::FLOW_RATE)]);
                        _response.vector.push_back(_flow_rate.bytes[6 - 2 * (reg - FlowMeter::FLOW_RATE)]);
                        break;
                    case FlowMeter::NET_TOTALIZER:
                        _flowmeter_net_totalizer.value += 11.1;
                    case FlowMeter::NET_TOTALIZER + 1:
                    case FlowMeter::NET_TOTALIZER + 2:
                    case FlowMeter::NET_TOTALIZER + 3:
                        _response.vector.push_back(
                                _flowmeter_net_totalizer.bytes[7 - 2 * (reg - FlowMeter::NET_TOTALIZER)]);
                        _response.vector.push_back(
                                _flowmeter_net_totalizer.bytes[6 - 2 * (reg - FlowMeter::NET_TOTALIZER)]);
                        break;
                    case FlowMeter::POSITIVE_TOTALIZER:
                    case FlowMeter::POSITIVE_TOTALIZER + 1:
                    case FlowMeter::POSITIVE_TOTALIZER + 2:
                    case FlowMeter::POSITIVE_TOTALIZER + 3:
                        _response.vector.push_back(
                                _flowmeter_net_totalizer
                                        .bytes[7 - 2 * (reg - FlowMeter::POSITIVE_TOTALIZER)]);
                        _response.vector.push_back(
                                _flowmeter_net_totalizer
                                        .bytes[6 - 2 * (reg - FlowMeter::POSITIVE_TOTALIZER)]);
                        break;
                    case FlowMeter::NEGATIVE_TOTALIZER:
                    case FlowMeter::NEGATIVE_TOTALIZER + 1:
                    case FlowMeter::NEGATIVE_TOTALIZER + 2:
                    case FlowMeter::NEGATIVE_TOTALIZER + 3: {
                        Transports::BytesValue<double> nt;
                        nt.value = -_flowmeter_net_totalizer.value;
                        _response.vector.push_back(nt.bytes[7 - 2 * (reg - FlowMeter::NEGATIVE_TOTALIZER)]);
                        _response.vector.push_back(nt.bytes[6 - 2 * (reg - FlowMeter::NEGATIVE_TOTALIZER)]);
                    } break;
                    case FlowMeter::SIGNAL_STRENGTH:
                        _response.vector.push_back(_signal.bytes[1]);
                        _response.vector.push_back(_signal.bytes[0]);
                        _signal.value += 1;
                        break;
                    default:
                        _response.vector.push_back(i);
                        _response.vector.push_back(i + 1);
                }
            }
            break;
        }
        case MPU::PRESET_HOLDING_REGISTER: {
            uint16_t reg = parser.read<uint16_t>();
            uint16_t reg_len = parser.read<uint16_t>();

            _response.vector.push_back(parser.func());
            _response.write(reg);
            _response.write(reg_len);

            break;
        }
        default:
            throw std::runtime_error(fmt::format("Response for function {} not implemented", parser.func()));
    }
    _response.writeCRC();
}
