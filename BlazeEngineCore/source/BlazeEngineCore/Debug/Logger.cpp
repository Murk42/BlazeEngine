#include "pch.h"
#include "BlazeEngineCore/Debug/Logger.h"

namespace Blaze::Debug::Logger
{
	void AddLoggerListener(Debug::LoggerListener* listener)
	{
		//std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		blazeEngineCoreContext.loggerListeners.AddFront(listener);
	}
	void RemoveLoggerListener(Debug::LoggerListener* listener)
	{
		//std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		blazeEngineCoreContext.loggerListeners.EraseOne([=](auto other) { return other == listener; });
	}

	void AddOutputStream(WriteStream& stream)
	{
		//std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		blazeEngineCoreContext.loggerOutputStreams.AddBack(BlazeEngineCoreContext::LoggerOutputStreamData({ &stream }));
	}
	void RemoveOutputStream(WriteStream& stream)
	{
		//std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		for (uint i = 0; i < blazeEngineCoreContext.loggerOutputStreams.Count(); ++i)
			if (blazeEngineCoreContext.loggerOutputStreams[i].writeStream == &stream)
				blazeEngineCoreContext.loggerOutputStreams.EraseAt(i);		
	}
	Result AddOutputFile(const Path& path)
	{
		//std::lock_guard lk{ blazeEngineCoreContext.contextMutex };

		if (!blazeEngineCoreContext.loggerOutputFiles.Find(path).IsNull())
			return Result();

		File file{ path, FileAccessPermission::Write };

		if (!file.IsOpen())
			return BLAZE_ERROR_RESULT("Blaze Engine Core", "Blaze::Debug::Logger::AddOutputFile failed because the file couldn't be opened");

		blazeEngineCoreContext.loggerOutputFiles.Insert(path, std::move(file));

		return Result();
	}
	Result RemoveOutputFile(const Path& path)
	{
		blazeEngineCoreContext.loggerOutputFiles.Erase(path);
		return Result();
	}
	
	static void WriteToStreams(StringViewUTF8 string)
	{
		//std::unique_lock lk{ blazeEngineCoreContext.contextMutex, std::try_to_lock };

		for (auto& stream : blazeEngineCoreContext.loggerOutputStreams)
		{
			stream.writeStream->Write(string.Buffer(), string.BufferSize() - 1);
			char endln = '\n';
			stream.writeStream->Write(&endln, 1);
		}

		for (auto& stream : blazeEngineCoreContext.loggerOutputFiles)
		{
			stream.value.file.Write(string.Buffer(), string.BufferSize() - 1);
			char endln = '\n';
			stream.value.file.Write(&endln, 1);
		}
	}

	void ProcessResult(Result&& result)
	{
		bool suppress = false;
		
		for (auto& listener : blazeEngineCoreContext.loggerListeners)
		{
			listener->NewResult(result);

			if (listener->IsSuppressing())
			{
				suppress = true;
				break;
			}
		}

		if (!suppress)
		{
			if (result.HighestLogType() >= LogType::Error)
				WriteToStreams(result.ToStringVerbose());
			else
				WriteToStreams(result.ToString());
		}

		if (result.HighestLogType() == LogType::Fatal)
		{
#ifdef _DEBUG				
			Breakpoint();
#endif
		}

		result.ClearSilent();
	}

	void LogDebug(StringUTF8&& source, StringUTF8&& message)
	{
		ProcessResult(BLAZE_DEBUG_RESULT(std::move(source), std::move(message)));
	}
	void LogInfo(StringUTF8&& source, StringUTF8&& message)
	{
		ProcessResult(BLAZE_INFO_RESULT(std::move(source), std::move(message)));
	}
	void LogWarning(StringUTF8&& source, StringUTF8&& message)
	{
		ProcessResult(BLAZE_WARNING_RESULT(std::move(source), std::move(message)));
	}
	void LogError(StringUTF8&& source, StringUTF8&& message)
	{
		ProcessResult(BLAZE_ERROR_RESULT(std::move(source), std::move(message)));
	}
	void LogFatal(StringUTF8&& source, StringUTF8&& message)
	{
		ProcessResult(BLAZE_FATAL_RESULT(std::move(source), std::move(message)));
	}
}