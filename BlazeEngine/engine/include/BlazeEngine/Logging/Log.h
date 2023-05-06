#pragma once

namespace Blaze
{
	enum class LogType
	{
		Debug,
		Info,
		Warning,
		Error,
		Fatal,
	};

	class BLAZE_API Log
	{
	public:				
		Log(const Log& log);
		Log(Log&&) noexcept;		
		Log(LogType type, Path filePath, uint line, String functionName, String source, String message);

		String ToString() const;

		inline uint GetThreadID() const { return threadID; }		
		inline LogType GetType() const { return type; }

		const Path& GetFilePath() const;
		StringView GetFunctionName() const;
		uint GetLine() const;		

		inline const String& GetSource() const { return source; }
		inline const String& GetMessage() const { return message; }		

		bool operator==(const Log&);
		bool operator!=(const Log&);

		Log& operator=(const Log&);
		Log& operator=(Log&&) noexcept;
	public:
		uint threadID;				
		TimePoint time;

		LogType type;

		Path filePath;
		String functionName;
		uint line;
		 
		String source;
		String message;				
	};
}

BLAZE_API Blaze::Path FILE_PATH(const char* macro);
#define BLAZE_FILE_NAME (::FILE_PATH(__FILE__))
#define BLAZE_FUNCTION_NAME __FUNCTION__
#define BLAZE_FILE_LINE ((uint)__LINE__)

#define BLAZE_DEBUG_LOG(source, message)	::Blaze::Log(::Blaze::LogType::Debug	, BLAZE_FILE_NAME, BLAZE_FILE_LINE, BLAZE_FUNCTION_NAME, source, message)
#define BLAZE_INFO_LOG(source, message)		::Blaze::Log(::Blaze::LogType::Info		, BLAZE_FILE_NAME, BLAZE_FILE_LINE, BLAZE_FUNCTION_NAME, source, message)
#define BLAZE_WARNING_LOG(source, message)	::Blaze::Log(::Blaze::LogType::Warning	, BLAZE_FILE_NAME, BLAZE_FILE_LINE, BLAZE_FUNCTION_NAME, source, message)
#define BLAZE_ERROR_LOG(source, message)	::Blaze::Log(::Blaze::LogType::Error	, BLAZE_FILE_NAME, BLAZE_FILE_LINE, BLAZE_FUNCTION_NAME, source, message)
#define BLAZE_FATAL_LOG(source, message)	::Blaze::Log(::Blaze::LogType::Fatal	, BLAZE_FILE_NAME, BLAZE_FILE_LINE, BLAZE_FUNCTION_NAME, source, message)