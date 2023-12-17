#pragma once

namespace Blaze::Debug
{
	enum class LogType
	{
		Info,
		Warning,
		Error,
		Fatal,
		Debug,
	};

	class BLAZE_CORE_API Log
	{
	public:				
		Log() = delete;
		Log(const Log& log);
		Log(Log&&) noexcept;		
		Log(LogType type, String source, String message);

		String ToString() const;
		String ToStringVerbose() const;

		inline LogType GetType() const { return type; }
		inline const String& GetSource() const { return source; }
		inline const String& GetMessage() const { return message; }		

		inline TimePoint GetTimePoint() const { return time; }
		inline uint GetThreadID() const { return threadID; }		
		inline const Callstack& GetCallstack() const { return callstack; }		

		bool operator==(const Log&);
		bool operator!=(const Log&);

		Log& operator=(const Log&);
		Log& operator=(Log&&) noexcept;
	public:
		LogType type;				 
		String source;
		String message;				

		TimePoint time;
		uint threadID;				
		Callstack callstack;
	};
}

#define BLAZE_DEBUG_LOG(source, message)	::Blaze::Debug::Log(::Blaze::Debug::LogType::Debug	 , source, message)
#define BLAZE_INFO_LOG(source, message)		::Blaze::Debug::Log(::Blaze::Debug::LogType::Info	 , source, message)
#define BLAZE_WARNING_LOG(source, message)	::Blaze::Debug::Log(::Blaze::Debug::LogType::Warning , source, message)
#define BLAZE_ERROR_LOG(source, message)	::Blaze::Debug::Log(::Blaze::Debug::LogType::Error	 , source, message)
#define BLAZE_FATAL_LOG(source, message)	::Blaze::Debug::Log(::Blaze::Debug::LogType::Fatal	 , source, message)