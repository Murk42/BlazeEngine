#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/Utilities/TimePoint.h"
#include "BlazeEngineCore/Debug/Callstack.h"

namespace Blaze::Debug
{
	enum class LogType
	{
		Info = 0,
		Warning = 1,
		Error = 2,
		Fatal = 3,
		Debug = 4,
	};

	class BLAZE_CORE_API Log
	{
	public:				
		Log();
		Log(const Log& log);
		Log(Log&&) noexcept;		
		Log(LogType type, StringUTF8 source, StringUTF8 message);

		StringUTF8 ToString() const;
		StringUTF8 ToStringVerbose() const;

		inline LogType GetType() const { return type; }
		inline StringViewUTF8 GetSource() const { return source; }
		inline StringViewUTF8 GetMessage() const { return message; }		

		inline TimePoint GetTimePoint() const { return time; }
		inline uint GetThreadID() const { return threadID; }		
		inline const Callstack& GetCallstack() const { return callstack; }		

		bool operator==(const Log&);
		bool operator!=(const Log&);

		Log& operator=(const Log&);
		Log& operator=(Log&&) noexcept;
	public:
		LogType type;				 
		StringUTF8 source;
		StringUTF8 message;				

		TimePoint time;
		uint threadID;				
		Callstack callstack;
	};
}