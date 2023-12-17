#pragma once
#include "BlazeEngineCore/File/File.h"

namespace Blaze::Debug::Logger
{
	BLAZE_CORE_API void ProcessString(StringView string);
	BLAZE_CORE_API void ProcessLog(const Log& log);	
	BLAZE_CORE_API void ProcessResult(Result&& result);	
	
	BLAZE_CORE_API void AddOutputStream(WriteStream& stream);
	BLAZE_CORE_API void RemoveOutputStream(WriteStream& stream);
	BLAZE_CORE_API void AddOutputFile(const Path& path);	
	BLAZE_CORE_API void RemoveOutputFile(const Path& path);	
	
	BLAZE_CORE_API void LogDebug(String&& source, String&& message);
	BLAZE_CORE_API void LogInfo(String&& source, String&& message);
	BLAZE_CORE_API void LogWarning(String&& source, String&& message);
	BLAZE_CORE_API void LogError(String&& source, String&& message);
	BLAZE_CORE_API void LogFatal(String&& source, String&& message);
}