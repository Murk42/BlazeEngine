#include "BlazeEngine/Debug/Logger.h"

#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Utilities/Time.h"

#include "BlazeEngine/File/File.h"

#include "BlazeEngine/Debug/Breakpoint.h"

namespace Blaze
{				
	static File logFile;
	static bool printToConsole = true;	


	namespace Debug::Logger
	{		
		void AddLoggerListener(Debug::LoggerListener* listener)
		{
			engineData->loggerListeners.AddFront(listener);
		}
		
		void RemoveLoggerListener(Debug::LoggerListener* listener)
		{					
			engineData->loggerListeners.EraseOne([=](auto other) { return other == listener; });
		}
		/*
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
					
			engineData->OpenGLResult = Result(Log(logType, "OpenGL API", message));			
		}
		*/
		void SetOutputFile(const Path& path)
		{
			if (logFile.IsOpen())
				logFile.Close();

			logFile.Open(path, FileAccessPermission::Write);
		}
		void PrintLogsToConsole(bool print)
		{
			printToConsole = print;
		}

#define LOGGER_BROADCAST(x)								\
	{bool supress = false;								\
	for (auto& listener : engineData->loggerListeners)	\
	{													\
		listener-> x ;									\
														\
		if (listener->IsSupressing())					\
		{												\
			supress = true;								\
			break;										\
		}												\
	}}

		void ProcessString(StringView string)
		{	
			LOGGER_BROADCAST(NewString(string))

			if (printToConsole)
				Console::WriteLine(string);			

			if (logFile.IsOpen())
			{
				logFile.Write(string.Ptr(), string.Size());
				char endln = '\n';
				logFile.Write(&endln, 1);
			}
		}
		void ProcessLog(const Log& log)
		{
			LOGGER_BROADCAST(NewLog(log))

			if (log.GetType() >= LogType::Error)
				ProcessString(log.ToStringVerbose());
			else
				ProcessString(log.ToString());

			if (log.GetType() == LogType::Fatal)
			{
#ifdef _DEBUG				
				Breakpoint();
#else
				exit(1);
#endif
			}							
		}

		void ProcessResult(Result&& result)
		{			
			LOGGER_BROADCAST(NewResult(result))

				if (result.HighestLogType() >= LogType::Error)
					ProcessString(result.ToStringVerbose());
				else
					ProcessString(result.ToString());

			if (result.HighestLogType() == LogType::Fatal)
			{
#ifdef _DEBUG				
				Breakpoint();
#else
				exit(1);
#endif
			}

			result.ClearSilent();
		}

		void LogDebug(String&& source, String&& message)
		{
			ProcessResult(BLAZE_DEBUG_RESULT(std::move(source), std::move(message)));
		}
		void LogInfo(String&& source, String&& message)
		{
			ProcessResult(BLAZE_INFO_RESULT(std::move(source), std::move(message)));
		}
		void LogWarning(String&& source, String&& message)
		{
			ProcessResult(BLAZE_WARNING_RESULT(std::move(source), std::move(message)));
		}
		void LogError(String&& source, String&& message) 
		{ 
			ProcessResult(BLAZE_ERROR_RESULT(std::move(source), std::move(message))); 
		}
		void LogFatal(String&& source, String&& message) 
		{ 
			ProcessResult(BLAZE_FATAL_RESULT(std::move(source), std::move(message))); 
		}		
	}
}