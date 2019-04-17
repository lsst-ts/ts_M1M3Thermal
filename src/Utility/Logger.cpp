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

#include "Logger.h"

#define COLOR_NORMAL  "\x1B[0m"
#define COLOR_RED     "\x1B[31m"
#define COLOR_GREEN   "\x1B[32m"
#define COLOR_YELLOW  "\x1B[33m"
#define COLOR_BLUE    "\x1B[34m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN    "\x1B[36m"
#define COLOR_WHITE   "\x1B[37m"

#define BUFFER_SIZE 1024

namespace LSST {
namespace TS {
namespace MTM1M3TS {

Logger::Logger() {
	LOGGER_LEVEL = LoggerLevels::None;
}

void Logger::SetLevel(LoggerLevels level) {
	LOGGER_LEVEL = level;
}

void Logger::Trace(const char* format, ...) {
	if (LOGGER_LEVEL > LoggerLevels::Trace) {
		return;
	}
	char buffer[BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	Logger::log(COLOR_WHITE, "TRACE", buffer);
	va_end(args);
}

void Logger::Debug(const char* format, ...) {
	if (LOGGER_LEVEL > LoggerLevels::Debug) {
		return;
	}
	char buffer[BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	Logger::log(COLOR_CYAN, "DEBUG", buffer);
	va_end(args);
}

void Logger::Info(const char* format, ...) {
	if (LOGGER_LEVEL > LoggerLevels::Info) {
		return;
	}
	char buffer[BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	Logger::log(COLOR_GREEN, "INFO", buffer);
	va_end(args);
}

void Logger::Warn(const char* format, ...) {
	if (LOGGER_LEVEL > LoggerLevels::Warn) {
		return;
	}
	char buffer[BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	Logger::log(COLOR_YELLOW, "WARN", buffer);
	va_end(args);
}

void Logger::Error(const char* format, ...) {
	if (LOGGER_LEVEL > LoggerLevels::Error) {
		return;
	}
	char buffer[BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	Logger::log(COLOR_RED, "ERROR", buffer);
	va_end(args);
}

void Logger::Fatal(const char* format, ...) {
	if (LOGGER_LEVEL > LoggerLevels::Fatal) {
		return;
	}
	char buffer[BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	Logger::log(COLOR_MAGENTA, "FATAL", buffer);
	va_end(args);
}

void Logger::log(const char* color, const char* level, const char* message) {
	std::time_t t = time(0);
	struct tm* now = localtime(&t);
	printf("%s%d-%02d-%02dT%02d:%02d:%02d\t%s\t%s%s \n", color, (now->tm_year + 1900), (now->tm_mon + 1), now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, level, message, COLOR_NORMAL);
}

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */
