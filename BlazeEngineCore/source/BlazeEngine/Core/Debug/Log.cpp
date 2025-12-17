#include "pch.h"
#include "BlazeEngine/Core/Debug/Log.h"
#include "BlazeEngine/Core/BlazeEngineCoreContext.h"

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
	Log::Log(LogType type, u8StringView source, u8StringView message, Callstack callstack) :
		threadID(GetThisThreadID()), callstack(std::move(callstack)),
		type(type),	source(std::move(source)), message(std::move(message))
	{
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