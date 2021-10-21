#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/Utilities/Time.h"
#include <thread> 

#include <string.h>

BLAZE_API const char* FILE_NAME(const char* macro);
#define BLAZE_FILE_NAME (::FILE_NAME(__FILE__))
#define BLAZE_FUNCTION_NAME __FUNCTION__
#define BLAZE_FILE_LINE __LINE__

namespace Blaze
{	
	enum class LogType 
	{
		Error,
		Info,
		Warning
	};

	class BLAZE_API Log
	{
		const std::thread::id threadID;
		Time time;

		LogType type;

		String filePath;
		String functionName;
		uint line;

		String source;
		String message;		
	public:					
		Log(LogType type, String&& filePath, String&& functionName, uint line, String&& source, String&& message);

		String FormatString();

		inline uint GetThreadID() const;
		inline const LogType GetType() const { return type; }

		inline const String& GetFilePath() const { return filePath; }
		inline const String& GetFunctionName() const { return functionName; }
		inline uint GetLine() const { return line; }

		inline const String& GetSource() const { return source; }
		inline const String& GetMessage() const { return message; }

		bool operator==(const Log&);
		bool operator!=(const Log&);
	};

	enum class LoggingPolicy
	{
		WaitForFlush,
		PrintInstantly
	};

	namespace Logger
	{				
		void BLAZE_API FlushLogs();		
		void BLAZE_API AddLog(LogType type, String&& fileName, String&& functionName, uint line, String&& source, String&& message);				
		void BLAZE_API SetLoggingPolicy(LoggingPolicy policy);		
	};

#define BLAZE_INFO_LOG(source, message) ::Blaze::Logger::AddLog(LogType::Info, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, source, message)
#define BLAZE_WARNING_LOG(source, message) ::Blaze::Logger::AddLog(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, source, message)
#define BLAZE_ERROR_LOG(source, message) ::Blaze::Logger::AddLog(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, source, message)
}