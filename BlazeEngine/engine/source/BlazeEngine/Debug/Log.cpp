#include "BlazeEngine/Debug/Log.h"
#include "BlazeEngine/Utilities/Threading.h"

namespace Blaze::Debug
{	
	Log::Log(const Log& log) :
		threadID(log.threadID), time(log.time), callstack(log.callstack),
		type(log.type), source(log.source), message(log.message)
	{
	}
	Log::Log(Log&& log) noexcept :
		threadID(log.threadID), time(std::move(log.time)), callstack(std::move(log.callstack)),
		type(log.type),	source(std::move(log.source)), message(std::move(log.message))
	{		
	}
	Log::Log(LogType type, String source, String message) :
		threadID(Threading::GetCurrentThreadID()), time(TimePoint::GetWorldTime()), callstack(),
		type(type),	source(std::move(source)), message(std::move(message))
	{
	}

	String Log::ToString() const
	{
		String timeStr = TimePoint::GetWorldTime().FormatString("%D %T");
		String typeStr;

		switch (type)
		{
		case Blaze::Debug::LogType::Debug: typeStr = "DEBUG"; break;
		case Blaze::Debug::LogType::Error: typeStr = "ERROR";  break;
		case Blaze::Debug::LogType::Info: typeStr = "INFO"; break;
		case Blaze::Debug::LogType::Warning: typeStr = "WARNING"; break;
		case Blaze::Debug::LogType::Fatal: typeStr = "FATAL"; break;
		}

		String out = "[" + timeStr + "] [" + typeStr + "] " + message;

		return out;
	}
	String Log::ToStringVerbose() const
	{
		String string = ToString() + "\n\n";

		for (auto& frame : callstack)		
			string += frame.FilePath().FileName() + ": " + StringParsing::Convert(frame.FileLine()) + " 0x" + StringParsing::Convert((size_t)frame.Address(), 16) + " " + frame.LocationName() + "\n";		

		return string;
	}


	bool Log::operator==(const Log& log)
	{			
		return callstack == log.callstack;
	}
	
	bool Log::operator!=(const Log& log)
	{
		return callstack != log.callstack;
	}
	Log& Log::operator=(const Log& log)
	{
		threadID = log.threadID;
		time = log.time;
		type = log.type;
		callstack = log.callstack;
		source = log.source;
		message = log.message;

		return *this;
	}
	Log& Log::operator=(Log&& log) noexcept
	{
		threadID = log.threadID;
		time = log.time;
		type = log.type;
		callstack = std::move(log.callstack);
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
