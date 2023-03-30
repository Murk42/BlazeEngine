#include "BlazeEngine/Logging/Logger.h"

#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Utilities/Time.h"
#include "BlazeEngine/Logging/LogListener.h"
#include "BlazeEngine/File/File.h"
#include "source/BlazeEngine/Internal/EngineData.h"

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

	void AddLogListener(LogListener* listener)
	{
		engineData->handlers.emplace(engineData->handlers.begin(), listener);
	}
	
	void RemoveLogListener(LogListener* listener)
	{
		auto it = std::find(engineData->handlers.begin(), engineData->handlers.end(), listener);
		engineData->handlers.erase(it);
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

		void RecordLog(String string)
		{
			if (printToConsole)
				Console::WriteLine(string);

			string += "\n";

			if (logFile.IsOpen())
				logFile.Write({ string.Ptr(), string.Size() });
		}

		void AddLog(const Log& log)
		{
			if (log.GetType() == LogType::Fatal)
			{
				RecordLog(log.FormatString());

#ifdef _DEBUG				
				DebugBreak();
#else
				exit(1);
#endif
			}

			if (log.GetType() == LogType::Fatal)
				throw log.FormatString();

			bool supress = false;
			for (auto& listener : engineData->handlers)
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

			RecordLog(log.FormatString());
		}

		void AddLog(LogType type, String&& fileName, String&& functionName, uint line, String&& source, String&& message)
		{	
			AddLog(Log(type, std::move(fileName), std::move(functionName), line, std::move(source), std::move(message)));
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