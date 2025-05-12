#pragma once

#include <u8lib/log.hpp>
#include <auxiliary/config/string.h>

#define AUXILIARY_LOG(level, format, ...)	\
	do {	\
		if (!u8lib::log::check_log_level(u8lib::log::LogLevel::level)) break;	\
		u8lib::log::log(MAKE_UTF8(AUXILIARY_FILE_LINE), reinterpret_cast<const char8_t*>(__FUNCTION__), u8lib::log::LogLevel::level, format, ##__VA_ARGS__);	\
	} while (0);

#define LOG_TRACE(format, ...)	AUXILIARY_LOG(trace, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...)	AUXILIARY_LOG(debug, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)	AUXILIARY_LOG(info, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)	AUXILIARY_LOG(warn, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)	AUXILIARY_LOG(error, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...)	AUXILIARY_LOG(fatal, format, ##__VA_ARGS__)