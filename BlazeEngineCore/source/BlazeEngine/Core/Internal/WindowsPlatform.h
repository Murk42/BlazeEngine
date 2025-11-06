#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Debug/Logger.h"

namespace Blaze::Windows
{
	// Parameters:
	// error - usually the result of GetLastError()
	// recursionIndex - for internal use
	String GetErrorString(uint32 error, uint recursionIndex = 0);
}

#define BLAZE_WINDOWS_DEBUG(message, ...) ::Blaze::Debug::Logger::LogDebug("Windows API", Format(message, __VA_ARGS__))
#define BLAZE_WINDOWS_INFO(message, ...) ::Blaze::Debug::Logger::LogInfo("Windows API", Format(message, __VA_ARGS__))
#define BLAZE_WINDOWS_WARNING(message, ...) ::Blaze::Debug::Logger::LogWarning("Windows API", Format(message, __VA_ARGS__))
#define BLAZE_WINDOWS_ERROR(message, ...) ::Blaze::Debug::Logger::LogError("Windows API", Format(message, __VA_ARGS__))
#define BLAZE_WINDOWS_FATAL(message, ...) ::Blaze::Debug::Logger::LogFatal("Windows API", Format(message, __VA_ARGS__))