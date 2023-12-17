#include "pch.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineCore/Debug/LoggerListener.h"
#include "BlazeEngineCore/File/File.h"

#include "BlazeEngineCore/Utilities/Time.h"

#include "BlazeEngineCore/Debug/Breakpoint.h"

namespace Blaze
{					
	namespace Debug::Logger
	{		
		void AddLoggerListener(Debug::LoggerListener* listener)
		{
			std::lock_guard<std::mutex> lg{ globalData.loggerMutex };
			globalData.loggerListeners.AddFront(listener);
		}		
		void RemoveLoggerListener(Debug::LoggerListener* listener)
		{	
			std::lock_guard<std::mutex> lg{ globalData.loggerMutex };
			globalData.loggerListeners.EraseOne([=](auto other) { return other == listener; });
		}				

		void AddOutputStream(WriteStream& stream)
		{ 			
			std::lock_guard<std::mutex> lg{ globalData.loggerMutex };
			globalData.loggerOutputStreams.Insert(&stream);			
		}
		void RemoveOutputStream(WriteStream& stream)
		{
			std::lock_guard<std::mutex> lg{ globalData.loggerMutex };
			globalData.loggerOutputStreams.Erase(&stream);			
		}
		void AddOutputFile(const Path& path)
		{						
			std::lock_guard<std::mutex> lg{ globalData.loggerMutex };
			auto& loggerOutputFileData = *globalData.loggerOutputFiles.AddFront();
			loggerOutputFileData.file.Open(path, FileAccessPermission::Write);
			loggerOutputFileData.path = path;			

			if (!loggerOutputFileData.file.IsOpen())
				globalData.loggerOutputFiles.EraseFirst();
		}
		void RemoveOutputFile(const Path& path)
		{			
			std::lock_guard<std::mutex> lg{ globalData.loggerMutex };
			globalData.loggerOutputFiles.EraseOne([&](const auto& it) {
				return it.path == path;
			});			
		}
		
		/*
			TS stands for thread safe, meaning it can only be called if according mutexes are locked.
		*/
		void WriteToStreams_TS(StringView string)
		{
			for (auto& stream : globalData.loggerOutputStreams)
			{
				stream->Write(string.Ptr(), string.Count());
				char endln = '\n';
				stream->Write(&endln, 1);
			}
			for (auto& stream : globalData.loggerOutputFiles)
			{
				stream.file.Write(string.Ptr(), string.Count());
				char endln = '\n';
				stream.file.Write(&endln, 1);
			}
		}

		void ProcessString(StringView string)
		{	
			std::lock_guard<std::mutex> lg{ globalData.loggerMutex };
			bool supress = false;

			for (auto& listener : globalData.loggerListeners)
			{
				listener->NewString(string);

				if (listener->IsSupressing())
				{
					supress = true;
					break;
				}
			}

			if (!supress)
				WriteToStreams_TS(string);
		}
		void ProcessLog(const Log& log)
		{
			std::lock_guard<std::mutex> lg{ globalData.loggerMutex };
			bool supress = false;

			for (auto& listener : globalData.loggerListeners)
			{
				listener->NewLog(log);

				if (listener->IsSupressing())
				{
					supress = true;
					break;
				}
			}

			if (!supress)
			{
				if (log.GetType() >= LogType::Error)
					WriteToStreams_TS(log.ToStringVerbose());					
				else
					WriteToStreams_TS(log.ToString());
			}					

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
			std::lock_guard<std::mutex> lg{ globalData.loggerMutex };

			bool supress = false;								

			for (auto& listener : globalData.loggerListeners)
			{
				listener->NewResult(result);

				if (listener->IsSupressing())
				{
					supress = true;
					break;
				}
			}

			if (!supress)
			{
				if (result.HighestLogType() >= LogType::Error)
					WriteToStreams_TS(result.ToStringVerbose());
				else
					WriteToStreams_TS(result.ToString());
			}

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