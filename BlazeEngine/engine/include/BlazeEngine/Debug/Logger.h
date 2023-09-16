#pragma once
#include "BlazeEngine/File/File.h"

namespace Blaze::Debug::Logger
{
	BLAZE_API void ProcessString(StringView string);
	BLAZE_API void ProcessLog(const Log& log);	
	BLAZE_API void ProcessResult(Result&& result);	

	BLAZE_API void SetOutputFile(const Path& path);
	BLAZE_API void PrintLogsToConsole(bool print);
	
	BLAZE_API void LogDebug(String&& source, String&& message);
	BLAZE_API void LogInfo(String&& source, String&& message);
	BLAZE_API void LogWarning(String&& source, String&& message);
	BLAZE_API void LogError(String&& source, String&& message);
	BLAZE_API void LogFatal(String&& source, String&& message);
}