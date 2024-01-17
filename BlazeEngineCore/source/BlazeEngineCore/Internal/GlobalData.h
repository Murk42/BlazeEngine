#pragma once

namespace Blaze
{
	namespace Debug
	{
		class LoggerListener;
	}

	struct GlobalDataCore
	{
		struct LoggerOutputFileData
		{
			File file;
			Path path;

			uint32 Hash() const
			{
				return file.Hash();
			}
		};

		Map<std::thread::id, uint> threadIDs;
		
		std::mutex loggerMutex;
		List<Debug::LoggerListener*> loggerListeners;		
		List<LoggerOutputFileData> loggerOutputFiles;
		Set<WriteStream*> loggerOutputStreams;		

		GlobalDataCore();
		~GlobalDataCore();
	};

	uint GetThisThreadID();

	extern GlobalDataCore globalDataCore;
}