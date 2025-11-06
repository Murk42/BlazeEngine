#include "pch.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/Core/Debug/LoggerListener.h"
#include "BlazeEngine/Core/Debug/Log.h"
#include "BlazeEngine/Core/Debug/Breakpoint.h"
#include "BlazeEngine/Core/BlazeEngineCoreContext.h"

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

	void AddOutputStream(WriteStream& stream, bool acceptsStyledText)
	{
		std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		blazeEngineCoreContext.loggerOutputStreams.AddBack(BlazeEngineCoreContext::LoggerOutputStreamData({ &stream, acceptsStyledText}));
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
			BLAZE_LOG_ERROR("Blaze::Debug::Logger::AddOutputFile failed because the file couldn't be opened");
			return;
		}

		blazeEngineCoreContext.loggerOutputFiles.Insert(path, std::move(file));
	}
	void RemoveOutputFile(const Path& path)
	{
		blazeEngineCoreContext.loggerOutputFiles.Erase(path);
	}

	void SetLogBreakpointMask(LogType mask)
	{
		blazeEngineCoreContext.logBreakpointMask = mask;
	}
	void SetLogCallstackPrintMask(LogType mask)
	{
		blazeEngineCoreContext.logCallstackPrintMask = mask;
	}

	void PassToStreamWithoutStyle(u8StringView string, WriteStream* stream, bool& insideStyleTag)
	{
		uintMem begin = 0;
		uintMem end = 0;

		while (begin != string.Count())
		{
			while (insideStyleTag)
			{
				if (string[begin] == '>')
					insideStyleTag = false;

				++begin;

				if (begin == string.Count())
					return;
			}

			end = begin;

			while (!insideStyleTag)
			{
				++end;

				if (end == string.Count())
					break;

				if (string[end] == '<')
					insideStyleTag = true;
			}

			stream->Write(string.Ptr() + begin, end - begin);

			begin = end;
		}
	}

	//This function is unsafe because it doesnt lock the blazeEngineCoreContext.contextMutex
	static void WriteToOutput_Unsafe(u8StringView string, bool styledText = false)
	{
		for (auto& stream : blazeEngineCoreContext.loggerOutputStreams)
			if (stream.acceptsStyledText || !styledText)
				stream.writeStream->Write(string.Ptr(), string.Count());
			else
				PassToStreamWithoutStyle(string, stream.writeStream, stream.insideStyleTag);

		for (auto& stream : blazeEngineCoreContext.loggerOutputFiles)
			if (styledText)
			{
				bool insideStyleTag = false;
				PassToStreamWithoutStyle(string, &stream.value.file, insideStyleTag);
			}
			else
				stream.value.file.Write(string.Ptr(), string.Count());
	}

	void ProcessLog(const Log& log, bool styledText)
	{
		bool suppress = false;

		for (auto& listener : blazeEngineCoreContext.loggerListeners)
		{
			listener->NewLog(log);

			if (listener->IsSuppressing())
			{
				suppress = true;
				break;
			}
		}

		if (!suppress)
		{
			std::unique_lock lk{ blazeEngineCoreContext.contextMutex };

			String time;
			StringView type;

			StringView typeStyleTag;
			switch (log.GetType())
			{
			case Blaze::Debug::LogType::Debug:   typeStyleTag = "<color=purple>"; type = "DEBUG"; break;
			case Blaze::Debug::LogType::Error:   typeStyleTag = "<color=red>";    type = "ERROR";  break;
			case Blaze::Debug::LogType::Info:    typeStyleTag = "<color=WHITE>";  type = "INFO"; break;
			case Blaze::Debug::LogType::Warning: typeStyleTag = "<color=yellow>"; type = "WARNING"; break;
			case Blaze::Debug::LogType::Fatal:   typeStyleTag = "<color=RED>";    type = "FATAL"; break;
			default: type = "INVALID TYPE"; break;
			}

			char typeFormatted[7];
			FormatInto<char>(typeFormatted, 7, "{^7}", type);

			WriteToOutput_Unsafe("[");
			WriteToOutput_Unsafe(typeStyleTag);
			WriteToOutput_Unsafe(typeFormatted);
			WriteToOutput_Unsafe("<color/>] ");
			WriteToOutput_Unsafe("(");
			WriteToOutput_Unsafe(time);
			WriteToOutput_Unsafe(") ");
			WriteToOutput_Unsafe(log.GetSource(), styledText);
			WriteToOutput_Unsafe(": ");
			WriteToOutput_Unsafe(log.GetMessage(), styledText);
			WriteToOutput_Unsafe("\n");

			if (static_cast<uint64>(log.GetType() & blazeEngineCoreContext.logCallstackPrintMask) != 0)
			{
				auto& callstack = log.GetCallstack();
				for (auto& frame : callstack)
				{
					char formattedName[40];
					FormatInto<char>(formattedName, _countof(formattedName), "{ <40}", frame.GetLocationName());

					char formattedAddress[18];
					FormatInto<char>(formattedAddress, _countof(formattedAddress), "{#18x}", frame.Address());

					char formattedFileLine[6];
					FormatInto<char>(formattedFileLine, _countof(formattedFileLine), "{<6}", frame.GetFileLine());

					char formattedFilePath[128];
					FormatInto<char>(formattedFilePath, _countof(formattedFilePath), "{}\0", frame.GetFilePath());

					WriteToOutput_Unsafe(formattedAddress);
					WriteToOutput_Unsafe(" - ");
					WriteToOutput_Unsafe(formattedName);
					WriteToOutput_Unsafe("| line ");
					WriteToOutput_Unsafe(formattedFileLine);
					WriteToOutput_Unsafe("| file \"");
					WriteToOutput_Unsafe(StringView(formattedFilePath, strnlen(formattedFilePath, _countof(formattedFilePath))));
					WriteToOutput_Unsafe("\"\n");
				}
				WriteToOutput_Unsafe("\n");
			}

			for (auto& stream : blazeEngineCoreContext.loggerOutputStreams)
				if (stream.acceptsStyledText)
					static_cast<StyledOutputStream*>(stream.writeStream)->ResetStyle();
		}

		if (static_cast<uint64>(log.GetType() & blazeEngineCoreContext.logBreakpointMask) != 0)
		{
#ifdef _DEBUG
			Breakpoint();
#endif
		}
	}

	void LogDebug(u8StringView&& source, u8StringView&& message, bool styledText)
	{
		ProcessLog(Log(LogType::Debug, std::move(source), std::move(message)), styledText);
	}
	void LogInfo(u8StringView&& source, u8StringView&& message, bool styledText)
	{
		ProcessLog(Log(LogType::Info, std::move(source), std::move(message)), styledText);
	}
	void LogWarning(u8StringView&& source, u8StringView&& message, bool styledText)
	{
		ProcessLog(Log(LogType::Warning, std::move(source), std::move(message)), styledText);
	}
	void LogError(u8StringView&& source, u8StringView&& message, bool styledText)
	{
		ProcessLog(Log(LogType::Error, std::move(source), std::move(message)), styledText);
	}
	void LogFatal(u8StringView&& source, u8StringView&& message, bool styledText)
	{
		ProcessLog(Log(LogType::Fatal, std::move(source), std::move(message)), styledText);
	}
}