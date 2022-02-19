#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Logger/Log.h"

BLAZE_API Blaze::String FILE_NAME(const char* macro);
#define BLAZE_FILE_NAME (::FILE_NAME(__FILE__))
#define BLAZE_FUNCTION_NAME __FUNCTION__
#define BLAZE_FILE_LINE __LINE__

namespace Blaze
{		
	enum class LoggingPolicy
	{
		WaitForFlush,
		PrintInstantly
	};

	namespace Logger
	{				
		void BLAZE_API FlushLogs();		
		void BLAZE_API AddLog(Log log);
		void BLAZE_API AddLog(LogType type, String&& fileName, String&& functionName, uint line, String&& source, String&& message);				
		void BLAZE_API SetLoggingPolicy(LoggingPolicy policy);				
	};

#define BLAZE_INFO_LOG(source, message) ::Blaze::Logger::AddLog(LogType::Info, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, source, message)
#define BLAZE_WARNING_LOG(source, message) ::Blaze::Logger::AddLog(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, source, message)
#define BLAZE_ERROR_LOG(source, message) ::Blaze::Logger::AddLog(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, source, message)
}