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

#ifndef UTILITY_LOGGER_H_
#define UTILITY_LOGGER_H_

#include <ctime>
#include <stdarg.h>
#include <stdio.h>

namespace LSST {
namespace TS {
namespace MTM1M3TS {

enum class LoggerLevels {
	Trace = 0,
	Debug = 10,
	Info = 20,
	Warn = 30,
	Error = 40,
	Fatal = 50,
	None = 100
};

class Logger {
private:
	LoggerLevels LOGGER_LEVEL;

public:
	Logger();

	void SetLevel(LoggerLevels level);

	void Trace(const char* format, ...);
	void Debug(const char* format, ...);
	void Info(const char* format, ...);
	void Warn(const char* format, ...);
	void Error(const char* format, ...);
	void Fatal(const char* format, ...);

private:
	void log(const char* color, const char* level, const char* message);
};

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */

extern LSST::TS::MTM1M3TS::Logger Log;

#endif /* UTILITY_LOGGER_H_ */