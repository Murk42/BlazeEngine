#include "pch.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineCore/Debug/LoggerListener.h"
#include "BlazeEngineCore/Debug/Breakpoint.h"
#include "BlazeEngineCore/Debug/Result.h"
#include "BlazeEngineCore/BlazeEngineCoreContext.h"
#include "BlazeEngineCore/DataStructures/ArrayImpl.h"
#include "BlazeEngineCore/DataStructures/ListImpl.h"
#include "BlazeEngineCore/DataStructures/MapImpl.h"
#include "BlazeEngineCore/DataStructures/DualListImpl.h"

namespace Blaze::Debug::Logger
{
	void AddLoggerListener(Debug::LoggerListener* listener)
	{
		std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		blazeEngineCoreContext.loggerListeners.AddFront(listener);
	}
	void RemoveLoggerListener(Debug::LoggerListener* listener)
	{
		std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		blazeEngineCoreContext.loggerListeners.EraseOne([=](auto other) { return other == listener; });
	}

	void AddOutputStream(WriteStream& stream)
	{
		std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		blazeEngineCoreContext.loggerOutputStreams.AddBack(BlazeEngineCoreContext::LoggerOutputStreamData({ &stream }));
	}
	void RemoveOutputStream(WriteStream& stream)
	{
		std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		for (uint i = 0; i < blazeEngineCoreContext.loggerOutputStreams.Count(); ++i)
			if (blazeEngineCoreContext.loggerOutputStreams[i].writeStream == &stream)
				blazeEngineCoreContext.loggerOutputStreams.EraseAt(i);		
	}
	void AddOutputFile(const Path& path)
	{
		std::lock_guard lk{ blazeEngineCoreContext.contextMutex };

		if (!blazeEngineCoreContext.loggerOutputFiles.Find(path).IsNull())
			return;

		File file{ path, FileAccessPermission::Write };

		if (!file.IsOpen())
		{
			BLAZE_ENGINE_CORE_ERROR("Blaze::Debug::Logger::AddOutputFile failed because the file couldn't be opened");
			return;
		}

		blazeEngineCoreContext.loggerOutputFiles.Insert(path, std::move(file));		
	}
	void RemoveOutputFile(const Path& path)
	{
		blazeEngineCoreContext.loggerOutputFiles.Erase(path);
	}
	
	static void WriteToStreams(StringViewUTF8 string)
	{
		std::unique_lock lk{ blazeEngineCoreContext.contextMutex, std::try_to_lock };

		for (auto& stream : blazeEngineCoreContext.loggerOutputStreams)
		{
			stream.writeStream->Write(string.Buffer(), string.BufferSize());
			char endln = '\n';
			stream.writeStream->Write(&endln, 1);
		}

		for (auto& stream : blazeEngineCoreContext.loggerOutputFiles)
		{
			stream.value.file.Write(string.Buffer(), string.BufferSize());
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

	template<>
	void LogDebug<>(StringUTF8&& source, StringUTF8&& message)
	{
		ProcessResult(Result(Log(LogType::Debug, std::move(source), std::move(message))));
	}
	template<>
	void LogInfo<>(StringUTF8&& source, StringUTF8&& message)
	{
		ProcessResult(Result(Log(LogType::Info, std::move(source), std::move(message))));
	}
	template<>
	void LogWarning<>(StringUTF8&& source, StringUTF8&& message)
	{
		ProcessResult(Result(Log(LogType::Warning, std::move(source), std::move(message))));
	}
	template<>
	void LogError<>(StringUTF8&& source, StringUTF8&& message)
	{
		ProcessResult(Result(Log(LogType::Error, std::move(source), std::move(message))));
	}
	template<>
	void LogFatal<>(StringUTF8&& source, StringUTF8&& message)
	{
		ProcessResult(Result(Log(LogType::Fatal, std::move(source), std::move(message))));
	}
}