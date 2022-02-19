#include "BlazeEngine/Logger/Logger.h"

#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Utilities/Time.h"
#include "BlazeEngine/Logger/LogListener.h"

#include "GL/glew.h"
#include <mutex>
#include <unordered_map>
#include <stack>

namespace Blaze
{		
	static std::mutex mutex;
	static std::unordered_map<std::thread::id, std::vector<Log*>> threadLogs;
	static std::vector<Log> logs;
	static std::vector<LogListener*> handlers;

	static LoggingPolicy policy = LoggingPolicy::PrintInstantly;	

	void AddLogListener(LogListener* listener)
	{
		handlers.emplace(handlers.begin(), listener);		
	}
	
	void RemoveLogListener(LogListener* listener)
	{
		auto it = std::find(handlers.begin(), handlers.end(), listener);
		handlers.erase(it);
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
			std::lock_guard<std::mutex> lk(mutex);
			for (auto& log : logs)
				;
				//TODO: SEND LOGS				
				//engine->AppInstance.ptr->NewLog(log);

			logs.clear();
			threadLogs.clear();
		}

		void BLAZE_API AddLog(Log log)
		{
			std::lock_guard<std::mutex> lk(mutex);

			if (log.GetType() == LogType::Fatal)			
				throw log.FormatString();			

			bool supress = false;
			for (auto& listener : handlers)
			{
				listener->AddLog(log);
				
				if (listener->DoesSupress())
				{
					supress = true;
					break;
				}
			}			

			if (supress)
				return;

			logs.emplace_back(log);

			switch (policy)
			{
			case Blaze::LoggingPolicy::WaitForFlush: {

				std::thread::id id = std::this_thread::get_id();
				threadLogs[id].emplace_back(&log);

				break;
			}
			case Blaze::LoggingPolicy::PrintInstantly: {

				Console::Write(log.FormatString());
				//if (log.GetType() == LogType::Error)
				//	throw log.GetMessage();

				break;
			}
			}						
		}

		void AddLog(LogType type, String&& fileName, String&& functionName, uint line, String&& source, String&& message)
		{			
			AddLog(Log(type, std::move(fileName), std::move(functionName), line, std::move(source), std::move(message)));
		}
		void SetLoggingPolicy(LoggingPolicy policy)
		{
			std::lock_guard<std::mutex> lk(mutex);
			policy = policy;
		}
	}
}

Blaze::String FILE_NAME(const char* macro)
{
	const char* ptr = nullptr;
	ptr = strrchr(macro, '\\') + 1;
	if (ptr == nullptr)	
		ptr = strrchr(macro, '/') + 1;
	
	if (ptr == nullptr)
		return Blaze::String(macro, strlen(macro));
	return Blaze::String(ptr, strlen(ptr));
}
