#include "pch.h"
#include "BlazeEngine/Internal/Libraries/sail.h"
#include "BlazeEngine/Internal/BlazeEngineContext.h"

#include "sail/sail.h"

namespace Blaze
{			
	static void SailLogger(SailLogLevel level, const char* file, int line, const char* format, va_list args)
	{		
		if (level == SAIL_LOG_LEVEL_INFO || level == SAIL_LOG_LEVEL_DEBUG)
			return;

		int size = _vscprintf(format, args);

		String message;

		if (size <= 0)		
			BLAZE_ENGINE_ERROR("Failed to format sail log");		
		else
		{
			message.Resize(size);
			if (vsprintf_s(message.Ptr(), message.Count() + 1, format, args) <= 0)
				BLAZE_ENGINE_ERROR("Failed to format sail log");
		}

		StringView levelString;
		auto logFunc = Debug::Logger::LogError;
		switch (level)
		{
		case SAIL_LOG_LEVEL_SILENCE: logFunc = Debug::Logger::LogInfo;levelString = "SILENCE"; break;
		case SAIL_LOG_LEVEL_ERROR:   logFunc = Debug::Logger::LogError;levelString = "ERROR"; break;
		case SAIL_LOG_LEVEL_WARNING: logFunc = Debug::Logger::LogWarning;levelString = "WARNING"; break;
		case SAIL_LOG_LEVEL_INFO:    logFunc = Debug::Logger::LogInfo;levelString = "INFO"; break;
		case SAIL_LOG_LEVEL_MESSAGE: logFunc = Debug::Logger::LogInfo;levelString = "MESSAGE"; break;
		case SAIL_LOG_LEVEL_DEBUG:   logFunc = Debug::Logger::LogInfo;levelString = "DEBUG"; break;
		case SAIL_LOG_LEVEL_TRACE:   logFunc = Debug::Logger::LogInfo;levelString = "TRACE"; break;
		default: levelString = "UNKNOWN"; break;
		}

		logFunc("sail", "sail (" + Path(file).FileName() + ", line " + StringParsing::Convert(line) + ") level " + levelString + ": \"" + message + "\"");
	}

	TimingResult InitializeSail()
	{
		Timing timing{ "sail" };
						
		sail_set_logger(SailLogger);
		sail_init();	

		BLAZE_ENGINE_CORE_INFO("Successfully initialized sail " SAIL_VERSION_STRING);

		return timing.GetTimingResult();
	}

	void TerminateSail()
	{				
	}
}