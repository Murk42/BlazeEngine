#include "BlazeEngine/Logging/Logger.h"

#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Utilities/Time.h"

#include "BlazeEngine/File/File.h"

#include "GL/glew.h"
#include <mutex>
#include <unordered_map>
#include <stack>

#include <Windows.h>
#include <debugapi.h>

namespace Blaze
{				
	static File logFile;
	static bool printToConsole = true;	

	void AddLoggerListener(LoggerListener* listener)
	{
		engineData->loggerListeners.emplace_front(listener);
	}
	
	void RemoveLoggerListener(LoggerListener* listener)
	{		
		auto it = std::find(engineData->loggerListeners.begin(), engineData->loggerListeners.end(), listener);
		engineData->loggerListeners.erase(it);
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
					
			engineData->OpenGLResult = Result(Log(logType, BLAZE_FILE_NAME, __LINE__, __FUNCTION__, "OpenGL API", message));			
		}

		static void ProcessString(String string)
		{			
			if (printToConsole)
				Console::WriteLine(string);

			string += "\n";

			if (logFile.IsOpen())
				logFile.Write({ string.Ptr(), string.Size() });
		}		

		void ProcessLog(const Log& log)
		{
			if (log.GetType() == LogType::Fatal)
			{				
				ProcessString(log.ToString());

#ifdef _DEBUG				
				DebugBreak();
#else
				exit(1);
#endif
			}
			else
			{				
				bool supress = false;
				for (auto& listener : engineData->loggerListeners)
				{
					listener->Add(log);

					if (listener->IsSupressing())
					{
						supress = true;
						break;
					}
				}

				if (supress)
					return;
				
				ProcessString(log.ToString());
			}
		}

		void ProcessResult(Result&& result)
		{			
			for (auto& listener : engineData->loggerListeners)
			{
				listener->Add(std::move(result));

				if (result.IsEmpty())
					break;
			}

			if (!result.IsEmpty())
			{
				String out = result.ToString();
				ProcessString(out);
				result.Clear();
			}
		}

		void ProcessResultSilent(Result&& result)
		{
			String out = result.ToString();
			ProcessString(out);
			result.Clear();
		}
		
		void SetOutputFile(const Path& path)
		{
			if (logFile.IsOpen())
				logFile.Close();

			logFile.Open(path, FileOpenMode::Write, FileOpenFlags::Create | FileOpenFlags::Truncate);
		}
		void PrintLogsToConsole(bool print)
		{
			printToConsole = print;
		}		
	}
}