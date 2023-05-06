#include "BlazeEngine/Logging/Log.h"
#include "BlazeEngine/Utilities/Threading.h"

namespace Blaze
{	
	Log::Log(const Log& log) :
		threadID(log.threadID), time(log.time), type(log.type),
		filePath(log.filePath), line(log.line), functionName(std::move(log.functionName)),
		source(log.source), message(log.message)
	{
	}
	Log::Log(Log&& log) noexcept :
		threadID(log.threadID), time(std::move(log.time)), type(log.type),
		filePath(std::move(log.filePath)), line(log.line), functionName(std::move(log.functionName)),
		source(std::move(log.source)), message(std::move(log.message))
	{		
	}
	Log::Log(LogType type, Path filePath, uint line, String functionName, String source, String message) :
		threadID(Threading::GetThreadID(std::this_thread::get_id())), time(TimePoint::GetWorldTime()), type(type),
		filePath(std::move(filePath)), line(line), functionName(std::move(functionName)),
		source(std::move(source)), message(std::move(message))
	{		
	} 

	String Log::ToString() const
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

		String out = "[" + timeStr + "] [" + typeStr + "] " + message;		

		return out;
	}

	const Path& Log::GetFilePath() const
	{
		return filePath;
	}

	StringView Log::GetFunctionName() const
	{
		return functionName;
	}

	uint Log::GetLine() const
	{
		return line;
	}

	bool Log::operator==(const Log& log)
	{		
		return filePath == log.filePath && line == log.line;		
	}
	
	bool Log::operator!=(const Log& log)
	{
		return filePath != log.filePath || line != log.line;
	}
	Log& Log::operator=(const Log& log)
	{
		threadID = log.threadID;
		time = log.time;
		type = log.type;
		filePath = log.filePath;
		line = log.line;
		functionName = log.functionName;
		source = log.source;
		message = log.message;

		return *this;
	}
	Log& Log::operator=(Log&& log) noexcept
	{
		threadID = log.threadID;
		time = log.time;
		type = log.type;
		filePath = std::move(log.filePath);
		line = log.line;
		functionName = std::move(log.functionName);
		source = std::move(log.source);
		message = std::move(log.message);

		return *this;
	}
}

Blaze::Path FILE_PATH(const char* macro)
{
	return Blaze::Path(macro);
	//const char* ptr = nullptr;
	//ptr = strrchr(macro, '\\') + 1;
	//if (ptr == nullptr)
	//	ptr = strrchr(macro, '/') + 1;
	//
	//if (ptr == nullptr)
	//	return Blaze::String(macro, strlen(macro));
	//return Blaze::String(ptr, strlen(ptr));
}
