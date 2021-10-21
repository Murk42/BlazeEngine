#include "BlazeEngine/Core/Logger.h"

#include "source/BlazeEngine/Internal/Engine.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Utilities/Time.h"

#include "GL/glew.h"

namespace Blaze
{		
	Log::Log(LogType type, String&& filePath, String&& functionName, uint line, String&& source, String&& message)
		: threadID(std::this_thread::get_id()), time(Time::GetWorldTime()), type(type), 
		filePath(std::move(filePath)), functionName(std::move(functionName)), line(line),
		source(std::move(source)), message(std::move(message))
	{
	}

	String Log::FormatString()
	{
		String timeStr = Time::GetWorldTime().FormatString("%D %T");
		String typeStr;
		switch (type)
		{
		case Blaze::LogType::Error: typeStr = "ERROR";  break;
		case Blaze::LogType::Info: typeStr = "INFO"; break;
		case Blaze::LogType::Warning: typeStr = "WARNING"; break;
		}
		return "[" + timeStr + "] [" + typeStr + "] " + filePath + ": " + message + "\n";
	}

	inline uint Log::GetThreadID() const
	{
		return engine->Core.GetThreadID(threadID);
	}

	bool Log::operator==(const Log& log)
	{
		return filePath == log.filePath && line == log.line;
	}

	bool Log::operator!=(const Log& log)
	{
		return filePath != log.filePath || line != log.line;
	}

	namespace Logger
	{		
		void OpenGLCallbackFunc(unsigned source, unsigned type, int id, unsigned severity, unsigned lenght, const char* message)
		{
			LogType logType;
			String _source;
			switch (source)
			{
			case GL_DEBUG_SOURCE_API: _source = "API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: _source = "Window System"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: _source = "Shader Compiler"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY: _source = "Third Party"; break;
			case GL_DEBUG_SOURCE_APPLICATION: _source = "Application"; break;
			default:
			case GL_DEBUG_SOURCE_OTHER: _source = "Other"; break;
			}			
			switch (type) {
			case GL_DEBUG_TYPE_ERROR:		
				logType = LogType::Error;
				break;				
			case GL_DEBUG_TYPE_MARKER:
				logType = LogType::Info;
				break;
			default:
				logType = LogType::Warning;
				break;
			}

			AddLog(logType, BLAZE_FILE_NAME, __FUNCTION__, __LINE__, "OpenGL", message);
		}		

		void FlushLogs()
		{
			std::lock_guard<std::mutex> lk(engine->Logger.mutex);
			for (auto& log : engine->Logger.logs)
				engine->AppInstance.ptr->NewLog(log);

			engine->Logger.logs.clear();
			engine->Logger.threadLogs.clear();
		}

		void AddLog(LogType type, String&& fileName, String&& functionName, uint line, String&& source, String&& message)
		{			
			std::lock_guard<std::mutex> lk(engine->Logger.mutex);
			Log& log = engine->Logger.logs.emplace_back(Log(type, std::move(fileName), std::move(functionName), line, std::move(source), std::move(message)));
			if (engine->Logger.policy == LoggingPolicy::WaitForFlush)
			{				
				std::thread::id id = std::this_thread::get_id();
				engine->Logger.threadLogs[id].emplace_back(&log);
			}
			else if (engine->Logger.policy == LoggingPolicy::PrintInstantly)
			{					
				Console::Write(log.FormatString());				
				if (log.GetType() == LogType::Error)				
					throw log.GetMessage();				
			}
		}
		void SetLoggingPolicy(LoggingPolicy policy)
		{
			std::lock_guard<std::mutex> lk(engine->Logger.mutex);
			engine->Logger.policy = policy;
		}
	}
}

const char* FILE_NAME(const char* macro)
{
	const char* ptr = nullptr;
	ptr = strrchr(macro, '\\') + 1;
	if (ptr == nullptr)
	{
		ptr = strrchr(macro, '/') + 1;
		if (ptr == nullptr)
			return ptr = macro;
	}
	return ptr;
}
