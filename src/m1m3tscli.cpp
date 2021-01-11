/*
 * Command line TS client.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software Systems.
 * This product includes software developed by the Vera C.Rubin Observatory Project
 * (https://www.lsst.org). See the COPYRIGHT file at the top-level directory of
 * this distribution for details of code ownership.
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

#include <cRIO/ThermalILC.h>
#include <cRIO/FPGA.h>
#include <spdlog/spdlog.h>
#include <CliApp.hpp>
#include <iostream>
#include <iomanip>

using namespace LSST::cRIO;

class M1M3TScli : public CliApp {
public:
    M1M3TScli(const char* description) : CliApp(description) {}

protected:
    void printUsage() override;
    void processArg(int opt, const char* optarg) override;
};

void M1M3TScli::printUsage() {
    std::cout << "M1M3 Thermal System command line tool. Access M1M3 Thermal System FPGA." << std::endl;
}

void M1M3TScli::processArg(int opt, const char* optarg) {
    switch (opt) {
        case 'h':
            printAppHelp();
            exit(EXIT_SUCCESS);
            break;
        default:
            std::cerr << "Unknown command: " << (char)(opt) << std::endl;
            exit(EXIT_FAILURE);
    }
}

M1M3TScli cli("M1M3 Thermal System Command Line Interface");

class PrintThermal : public ThermalILC {
protected:
    void processServerID(uint8_t address, uint64_t uniqueID, uint8_t ilcAppType, uint8_t networkNodeType,
                         uint8_t ilcSelectedOptions, uint8_t networkNodeOptions, uint8_t majorRev,
                         uint8_t minorRev, std::string firmwareName);

    void processServerStatus(uint8_t address, uint8_t mode, uint16_t status, uint16_t faults) override {}

    void processChangeILCMode(uint8_t address, uint16_t mode) override {}

    void processSetTempILCAddress(uint8_t address, uint8_t newAddress) override {}

    void processResetServer(uint8_t address) override {}

    void processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature, uint8_t fanRPM,
                              float absoluteTemperature) override {}
};

void PrintThermal::processServerID(uint8_t address, uint64_t uniqueID, uint8_t ilcAppType,
                                   uint8_t networkNodeType, uint8_t ilcSelectedOptions,
                                   uint8_t networkNodeOptions, uint8_t majorRev, uint8_t minorRev,
                                   std::string firmwareName) {
    std::cout << "Address:" << address << std::endl
              << "UniqueID:" << std::hex << std::setw(8) << std::setfill('0') << (uniqueID) << std::endl;
}

int _verbose = 0;

void _updateVerbosity(int newVerbose) {
    _verbose = newVerbose;
    spdlog::level::level_enum logLevel = spdlog::level::trace;

    switch (_verbose) {
        case 0:
            logLevel = spdlog::level::info;
        case 1:
            logLevel = spdlog::level::debug;
            break;
    }
    spdlog::set_level(logLevel);
}

int verbose(command_vec cmds) {
    switch (cmds.size()) {
        case 1:
            _updateVerbosity(std::stoi(cmds[0]));
        case 0:
            std::cout << "Verbosity level: " << _verbose << std::endl;
            break;
    }
    return 0;
}

command_t commands[] = {
#if 0
        {"close",
         [=](command_vec cmds) {
             closeFPGA();
             return 0;
         },
         "", 0, NULL, "Close FPGA connection"},
        {"info", [=](command_vec cmds) { return cli.info(cmds); }, "S?", 0, "<address>..", "Print ILC info"},
        {"open",
         [=](command_vec) {
             openFPGA();
             return 0;
         },
         "", 0, NULL, "Open FPGA"},
#endif
        {"verbose", &verbose, "?", 0, "<new level>", "Report/set verbosity level"},
        {NULL, NULL, NULL, 0, NULL, NULL}};

int main(int argc, char* const argv[]) { command_vec cmds = cli.init(commands, "hv", argc, argv); }
