#include "BlazeEngine/Logging/Log.h"
#include "BlazeEngine/Utilities/Threading.h"

namespace Blaze
{
	Log::Log(LogType type, String&& filePath, String&& functionName, uint line, String&& source, String&& message)
		: threadID(Threading::GetThreadID(std::this_thread::get_id())), time(TimePoint::GetWorldTime()), type(type),
		filePath(std::move(filePath)), functionName(std::move(functionName)), line(line),
		source(std::move(source)), message(std::move(message))
	{
	}

	String Log::FormatString()
	{
		String timeStr = TimePoint::GetWorldTime().FormatString("%D %T");
		String typeStr;
		switch (type)
		{
		case Blaze::LogType::Debug: typeStr = "DEBUG"; break;
		case Blaze::LogType::Error: typeStr = "ERROR";  break;
		case Blaze::LogType::Info: typeStr = "INFO"; break;
		case Blaze::LogType::Warning: typeStr = "WARNING"; break;
		case Blaze::LogType::Fatal: typeStr = "FATAL"; break;
		}
		return "[" + timeStr + "] [" + typeStr + "] " + filePath + "(" + functionName + "): " + message + " (line " + String::Convert(line) + ")";
	}

	bool Log::operator==(const Log& log)
	{
		return filePath == log.filePath && line == log.line;
	}

	bool Log::operator!=(const Log& log)
	{
		return filePath != log.filePath || line != log.line;
	}
}

Blaze::String FILE_NAME(const char* macro)
{
	const char* ptr = nullptr;
	ptr = strrchr(macro, '\\') + 1;
	if (ptr == nullptr)
		ptr = strrchr(macro, '/') + 1;

	if (ptr == nullptr)
		return Blaze::String(macro, strlen(macro));
	return Blaze::String(ptr, strlen(ptr));
}
