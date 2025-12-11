/*
 * FlowMeter MPU
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

#ifndef __TS_MPU_FLOWMETER__
#define __TS_MPU_FLOWMETER__

#include <cRIO/MPU.h>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Reads FlowMeter values.
 * [Documentation](https://rubinobs.atlassian.net/wiki/spaces/LTS/pages/50081742/Datasheets?preview=/50081742/50505733/FDT40%20Users%20Guide.pdf)
 * [Datasheets](https://confluence.lsstcorp.org/display/LTS/Datasheets).
 */
class FlowMeter : public cRIO::MPU {
public:
    /**
     * Flow Meter registers. 0-based, so -1 from what's in the documentation.
     */
    enum REGISTERS {
        FLOW_RATE = 1600,
        VELOCITY = 1604,
        NET_TOTALIZER = 2800,
        POSITIVE_TOTALIZER = 2804,
        NEGATIVE_TOTALIZER = 2808,
        SIGNAL_STRENGTH = 5500,
        TAG = 7000,
        PART_NUMBER = 7064,
        SERIAL_NUMBER = 7128,
        FIRMWARE_VERSION = 7192,
        CALIBRATION_DATE = 7256,
        DATE_CODE = 7320,
    };

    FlowMeter() : MPU(1) {}

    /**
     * Push class to readout identification registers.
     */
    void read_identification();

    /**
     * Push calls to readout telemetry FlowMeter registers.
     */
    void read_telemetry();

    uint16_t get_signal_strength() { return getRegister(SIGNAL_STRENGTH); }
    double get_flow_rate() { return _get_double_value(FLOW_RATE); }
    double get_velocity() { return _get_double_value(VELOCITY); }
    double get_net_totalizer() { return _get_double_value(NET_TOTALIZER); }
    double get_positive_totalizer() { return _get_double_value(POSITIVE_TOTALIZER); }
    double get_negative_totalizer() { return _get_double_value(NEGATIVE_TOTALIZER); }

    std::string get_tag() { return _get_string(TAG, 21); }
    std::string get_part_number() { return _get_string(PART_NUMBER, 21); }
    std::string get_serial_number() { return _get_string(SERIAL_NUMBER, 10); }
    std::string get_firmware_version() { return _get_string(FIRMWARE_VERSION, 10); }
    std::string get_calibration_date() { return _get_string(CALIBRATION_DATE, 10); }
    std::string get_date_code() { return _get_string(DATE_CODE, 10); }

private:
    float _get_float_value(uint16_t reg);
    double _get_double_value(uint16_t reg);

    std::string _get_string(uint16_t reg, uint8_t len);
};

/**
 * Add print() method to print readout values.
 */
class FlowMeterPrint : public FlowMeter {
public:
    void print_identification();
    void print();
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif /* __TS_MPU_FLOWMETER__ */
