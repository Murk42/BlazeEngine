#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Logging/Log.h"
#include "BlazeEngine/File/File.h"

namespace Blaze
{
	namespace Logger
	{		
		void BLAZE_API AddLog(Log log);
		void BLAZE_API AddLog(LogType type, String&& fileName, String&& functionName, uint line, String&& source, String&& message);		
		void BLAZE_API SetOutputFile(const Path& path);
		void BLAZE_API PrintLogsToConsole(bool print);
	};
}