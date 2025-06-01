#include "pch.h"
#include "BlazeEngineCore/Debug/Log.h"
#include "BlazeEngineCore/Format/Format.h"
#include "BlazeEngineCore/BlazeEngineCoreContext.h"

namespace Blaze::Debug
{
	Log::Log() :
		threadID(UINT_MAX), time(), callstack(),
		type(LogType::Info), source(), message()
	{
	}
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
	Log::Log(LogType type, StringUTF8 source, StringUTF8 message) :
		threadID(GetThisThreadID()), time(TimePoint::GetCurrentWorldTime()), callstack(),
		type(type),	source(std::move(source)), message(std::move(message))
	{
	}

	StringUTF8 Log::ToString() const
	{
		StringUTF8 timeStr = (StringUTF8)TimePoint::GetCurrentWorldTime().FormatString("%D %T");
		StringUTF8 typeStr;

		switch (type)
		{
		case Blaze::Debug::LogType::Debug: typeStr = "DEBUG"; break;
		case Blaze::Debug::LogType::Error: typeStr = "ERROR";  break;
		case Blaze::Debug::LogType::Info: typeStr = "INFO"; break;
		case Blaze::Debug::LogType::Warning: typeStr = "WARNING"; break;
		case Blaze::Debug::LogType::Fatal: typeStr = "FATAL"; break;
		}

		StringUTF8 out = "[" + timeStr + "] [" + typeStr + "] [" + source + "] " + message;

		return FormatUTF8("[{}] [{}] [{}] {}", timeStr, typeStr, source, message);
	}
	StringUTF8 Log::ToStringVerbose() const
	{
		StringUTF8 string = ToString() + "\n\n";


		uintMem maxWidth = 4;
		for (auto& frame : callstack)
		{
			StringUTF8 fileName = frame.FilePath().FileName();
			if (fileName.Empty())
				fileName = "Unknown";
			maxWidth = std::max(fileName.CharacterCount(), maxWidth);
		}

		string += FormatUTF8(FormatUTF8("|-{{:-^{}}}-|--------|----------\n", maxWidth), "");
		string += FormatUTF8(FormatUTF8("| {{: ^{}}} |  Line  | Function \n", maxWidth), "File");
		string += FormatUTF8(FormatUTF8("|-{{:-^{}}}-|--------|----------\n", maxWidth), "");

		StringUTF8 formatString = FormatUTF8("| {{:^{}}} | {{:>6}} | {{}}\n", maxWidth);

		for (auto& frame : callstack)
		{
			StringUTF8 path = frame.FilePath().FileName();
			if (path.Empty())
				path = "Unknown";
			string += FormatUTF8(formatString, path, frame.FileLine(), frame.LocationName());
		}

		string += FormatUTF8(FormatUTF8("|-{{:-^{}}}-|--------|----------\n", maxWidth), "");

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