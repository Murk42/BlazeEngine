#include "BlazeEngine/Core/Logger.h"
#include "GL/glew.h"

#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze
{
	Log::Log()
		: type(LogType::Message)
	{
	}
	Log::Log(LogType type, String&& source, String&& message)
		: type(type), source(std::move(source)), message(std::move(message))
	{
	}

	inline bool Log::Empty()
	{
		return source.Size() == 0 && message.Size() == 0;
	}

	bool Log::operator==(const Log& log)
	{
		return type == log.type && source == log.source && message == log.message;
	}

	bool Log::operator!=(const Log& log)
	{
		return type != log.type || source != log.source || message != log.message;
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
				logType = LogType::Message;
				break;
			default:
				logType = LogType::Warning;
				break;
			}

			AddLog(logType, std::move(_source), message);
		}

		void AddLog(LogType type, String&& source, String&& message)
		{			
			Log log = Log(type, std::move(source), std::move(message));
			engine->AppInstance.ptr->NewLog(log);
		}		
	}
}