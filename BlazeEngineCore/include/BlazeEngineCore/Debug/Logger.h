#pragma once
#include "BlazeEngineCore/File/File.h"

namespace Blaze::Debug::Logger
{
	BLAZE_CORE_API void ProcessString(StringViewUTF8 string);
	BLAZE_CORE_API void ProcessLog(const Log& log);	
	BLAZE_CORE_API void ProcessResult(Result&& result);	
	
	BLAZE_CORE_API void AddOutputStream(WriteStream& stream);
	BLAZE_CORE_API void RemoveOutputStream(WriteStream& stream);
	BLAZE_CORE_API void AddOutputFile(const Path& path);	
	BLAZE_CORE_API void RemoveOutputFile(const Path& path);	
	
	BLAZE_CORE_API void LogDebug(StringUTF8&& source, StringUTF8&& message);
	BLAZE_CORE_API void LogInfo(StringUTF8&& source, StringUTF8&& message);
	BLAZE_CORE_API void LogWarning(StringUTF8&& source, StringUTF8&& message);
	BLAZE_CORE_API void LogError(StringUTF8&& source, StringUTF8&& message);
	BLAZE_CORE_API void LogFatal(StringUTF8&& source, StringUTF8&& message);
}