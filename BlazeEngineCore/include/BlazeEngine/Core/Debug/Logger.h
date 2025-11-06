#pragma once
#include "BlazeEngine/Core/Common/Format.h"
#include "BlazeEngine/Core/Debug/Log.h"

namespace Blaze
{
	class Path;
	class WriteStream;
}

namespace Blaze::Debug
{
	class Log;
	class LoggerListener;
}

namespace Blaze::Debug::Logger
{
	BLAZE_API void AddLoggerListener(Debug::LoggerListener* listener);
	BLAZE_API void RemoveLoggerListener(Debug::LoggerListener* listener);

	BLAZE_API void AddOutputStream(WriteStream& stream, bool acceptsStyledText = false);
	BLAZE_API void RemoveOutputStream(WriteStream& stream);
	BLAZE_API void AddOutputFile(const Path& path);
	BLAZE_API void RemoveOutputFile(const Path& path);

	BLAZE_API void SetLogBreakpointMask(LogType mask);
	BLAZE_API void SetLogCallstackPrintMask(LogType mask);

	BLAZE_API void ProcessLog(const Log& log, bool styledText = true);

	BLAZE_API void LogDebug(u8StringView&& source, u8StringView&& message, bool styledText);
	BLAZE_API void LogInfo(u8StringView&& source, u8StringView&& message, bool styledText);
	BLAZE_API void LogWarning(u8StringView&& source, u8StringView&& message, bool styledText);
	BLAZE_API void LogError(u8StringView&& source, u8StringView&& message, bool styledText);
	BLAZE_API void LogFatal(u8StringView&& source, u8StringView&& message, bool styledText);
}

#define BLAZE_LOG_DEBUG(message, ...) ::Blaze::Debug::Logger::LogDebug(u8"Blaze Engine", ::Blaze::Format<char8_t>(message, __VA_ARGS__))
#define BLAZE_LOG_INFO(message, ...) ::Blaze::Debug::Logger::LogInfo(u8"Blaze Engine", ::Blaze::Format<char8_t>(message, __VA_ARGS__))
#define BLAZE_LOG_WARNING(message, ...) ::Blaze::Debug::Logger::LogWarning(u8"Blaze Engine", ::Blaze::Format<char8_t>(message, __VA_ARGS__))
#define BLAZE_LOG_ERROR(message, ...) ::Blaze::Debug::Logger::LogError(u8"Blaze Engine", ::Blaze::Format<char8_t>(message, __VA_ARGS__))
#define BLAZE_LOG_FATAL(message, ...) ::Blaze::Debug::Logger::LogFatal(u8"Blaze Engine", ::Blaze::Format<char8_t>(message, __VA_ARGS__))
