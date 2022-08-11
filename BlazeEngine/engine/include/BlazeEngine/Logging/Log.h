#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/Utilities/Time.h"

BLAZE_API Blaze::String FILE_NAME(const char* macro);
#define BLAZE_FILE_NAME (::FILE_NAME(__FILE__))
#define BLAZE_FUNCTION_NAME __FUNCTION__
#define BLAZE_FILE_LINE __LINE__

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
		uint threadID;
		TimePoint time;

		LogType type;

		String filePath;
		String functionName;
		uint line;

		String source;
		String message;
	public:
		Log(LogType type, String&& filePath, String&& functionName, uint line, String&& source, String&& message);

		String FormatString();

		inline uint GetThreadID() const { return threadID; }
		inline const LogType GetType() const { return type; }

		inline const String& GetFilePath() const { return filePath; }
		inline const String& GetFunctionName() const { return functionName; }
		inline uint GetLine() const { return line; }

		inline const String& GetSource() const { return source; }
		inline const String& GetMessage() const { return message; }

		bool operator==(const Log&);
		bool operator!=(const Log&);
	};
}