#pragma once
#include "BlazeEngine/File/File.h"

namespace Blaze
{
	namespace Logger
	{		
		BLAZE_API void ProcessLog(const Log& log);		
		BLAZE_API void ProcessResult(Result&& result);
		BLAZE_API void ProcessResultSilent(Result&& result);

		BLAZE_API void SetOutputFile(const Path& path);
		BLAZE_API void PrintLogsToConsole(bool print);		
	};
}