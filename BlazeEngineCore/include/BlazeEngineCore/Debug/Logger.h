#pragma once
#include "BlazeEngineCore/File/Path.h"
#include "BlazeEngineCore/File/Stream/Stream.h"
#include "BlazeEngineCore/Debug/Result.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"

namespace Blaze
{
	class WriteStream;
	class Path;
	class StringUTF8;
	class Result;
}

namespace Blaze::Debug
{	
	class LoggerListener;
}

namespace Blaze::Debug::Logger
{
	BLAZE_CORE_API void AddLoggerListener(Debug::LoggerListener* listener);
	BLAZE_CORE_API void RemoveLoggerListener(Debug::LoggerListener* listener);
		  
	BLAZE_CORE_API void AddOutputStream(WriteStream& stream);
	BLAZE_CORE_API void RemoveOutputStream(WriteStream& stream);
	BLAZE_CORE_API Result AddOutputFile(const Path& path);
	BLAZE_CORE_API Result RemoveOutputFile(const Path& path);
		  
	BLAZE_CORE_API void ProcessResult(Result&& result);
		  
	BLAZE_CORE_API void LogDebug(StringUTF8&& source, StringUTF8&& message);
	BLAZE_CORE_API void LogInfo(StringUTF8&& source, StringUTF8&& message);
	BLAZE_CORE_API void LogWarning(StringUTF8&& source, StringUTF8&& message);
	BLAZE_CORE_API void LogError(StringUTF8&& source, StringUTF8&& message);
	BLAZE_CORE_API void LogFatal(StringUTF8&& source, StringUTF8&& message);
}