#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/String/StringView.h"
#include "BlazeEngine/Core/Time/TimePoint.h"
#include "BlazeEngine/Core/Debug/Callstack.h"

namespace Blaze::Debug
{
	enum class LogType
	{
		Info = 1,
		Debug = 2,
		Warning = 4,
		Error = 8,
		Fatal = 16,
	};
	ENUM_CLASS_BITWISE_OPERATIONS(LogType)

	class BLAZE_API Log
	{
	public:
		Log();
		Log(const Log& log);
		Log(Log&&) noexcept;
		Log(LogType type, u8StringView source, u8StringView message, Callstack callstack = Callstack(0));

		inline LogType GetType() const { return type; }
		inline u8StringView GetSource() const { return source; }
		inline u8StringView GetMessage() const { return message; }
		inline ProgramTimePoint GetTimePoint() const { return time; }
		inline uint GetThreadID() const { return threadID; }
		inline const Callstack& GetCallstack() const { return callstack; }

		bool operator==(const Log&);
		bool operator!=(const Log&);

		Log& operator=(const Log&);
		Log& operator=(Log&&) noexcept;
	public:
		LogType type;
		u8String source;
		u8String message;

		ProgramTimePoint time;
		uint threadID;
		Callstack callstack;
	};
}