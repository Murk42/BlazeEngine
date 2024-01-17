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

#define BLAZE_DEBUG_LOG(source, message)	::Blaze::Debug::Log(::Blaze::Debug::LogType::Debug	 , source, message)
#define BLAZE_INFO_LOG(source, message)		::Blaze::Debug::Log(::Blaze::Debug::LogType::Info	 , source, message)
#define BLAZE_WARNING_LOG(source, message)	::Blaze::Debug::Log(::Blaze::Debug::LogType::Warning , source, message)
#define BLAZE_ERROR_LOG(source, message)	::Blaze::Debug::Log(::Blaze::Debug::LogType::Error	 , source, message)
#define BLAZE_FATAL_LOG(source, message)	::Blaze::Debug::Log(::Blaze::Debug::LogType::Fatal	 , source, message)