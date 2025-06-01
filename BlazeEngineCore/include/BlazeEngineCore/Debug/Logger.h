#pragma once
#include "BlazeEngineCore/Format/Format.h"

namespace Blaze
{
	class Path;
	class WriteStream;
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
	BLAZE_CORE_API void AddOutputFile(const Path& path);
	BLAZE_CORE_API void RemoveOutputFile(const Path& path);
		  
	BLAZE_CORE_API void ProcessResult(Result&& result);
		  
	template<Formatable ... Args>
	void LogDebug(StringUTF8&& source, StringUTF8&& message, const Args& ... args);
	template<Formatable ... Args>
	void LogInfo(StringUTF8&& source, StringUTF8&& message, const Args& ... args);
	template<Formatable ... Args>
	void LogWarning(StringUTF8&& source, StringUTF8&& message, const Args& ... args);
	template<Formatable ... Args>
	void LogError(StringUTF8&& source, StringUTF8&& message, const Args& ... args);
	template<Formatable ... Args>
	void LogFatal(StringUTF8&& source, StringUTF8&& message, const Args& ... args);

	template<>
	BLAZE_CORE_API void LogDebug<>(StringUTF8&& source, StringUTF8&& message);
	template<>
	BLAZE_CORE_API void LogInfo<>(StringUTF8&& source, StringUTF8&& message);
	template<>
	BLAZE_CORE_API void LogWarning<>(StringUTF8&& source, StringUTF8&& message);
	template<>
	BLAZE_CORE_API void LogError<>(StringUTF8&& source, StringUTF8&& message);
	template<>
	BLAZE_CORE_API void LogFatal<>(StringUTF8&& source, StringUTF8&& message);

	template<Formatable ...Args>
	void LogDebug(StringUTF8&& source, StringUTF8&& message, const Args& ...args)
	{
		LogDebug(std::move(source), FormatUTF8(message, args...));
	}
	template<Formatable ...Args>
	void LogInfo(StringUTF8&& source, StringUTF8&& message, const Args& ...args)
	{
		LogInfo(std::move(source), FormatUTF8(message, args...));
	}
	template<Formatable ...Args>
	void LogWarning(StringUTF8&& source, StringUTF8&& message, const Args& ...args)
	{
		LogWarning(std::move(source), FormatUTF8(message, args...));
	}
	template<Formatable ...Args>
	void LogError(StringUTF8&& source, StringUTF8&& message, const Args& ...args)
	{
		LogError(std::move(source), FormatUTF8(message, args...));
	}
	template<Formatable ...Args>
	void LogFatal(StringUTF8&& source, StringUTF8&& message, const Args& ...args)
	{
		LogFatal(std::move(source), FormatUTF8(message, args...));
	}

}