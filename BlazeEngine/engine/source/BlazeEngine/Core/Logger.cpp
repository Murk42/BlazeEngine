#include "BlazeEngine/Core/Logger.h"
#include "GL/glew.h"

#include "Engine.h"

namespace Blaze
{
	Log::Log(LogType type, String&& source, String&& message)
		: type(type), source(source), message(message)
	{
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

			AddLog(LogType::Error, std::move(_source), message);
		}

		void AddLog(LogType type, String&& source, String&& message)
		{
			std::lock_guard<std::mutex> lk(engine->Logger.queueMutex);
			engine->Logger.logs.emplace(type, std::move(source), std::move(message));
		}

		bool GetNextLog(Log& log)
		{
			std::lock_guard<std::mutex> lk(engine->Logger.queueMutex);
			if (engine->Logger.logs.size() == 0)
				return false;
			log = engine->Logger.logs.front();
			engine->Logger.logs.pop();
			return true;
		}
	}
}