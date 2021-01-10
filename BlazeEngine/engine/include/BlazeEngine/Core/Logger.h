#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/String.h"
#include <queue>
#include <mutex>

namespace Blaze
{	
	enum class LogType
	{
		Error,
		Message,
		Warning
	};

	struct BLAZE_API Log
	{
		LogType type;
		String source;
		String message;
			
		Log();
		Log(LogType, String&&, String&&);

		bool operator==(const Log&);
		bool operator!=(const Log&);
	};

	namespace Logger
	{				
		void BLAZE_API AddLog(LogType type, String&& source, String&& message);		
		bool BLAZE_API GetNextLog(Log& log);
	};
}