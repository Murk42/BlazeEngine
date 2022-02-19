#pragma once
#include "BlazeEngine/Logger/Log.h"
#include <vector>

namespace Blaze
{
	class BLAZE_API LogListener
	{
	public:
		LogListener();
		LogListener(const LogListener&) = delete;		
		~LogListener();

		void SupressLogs(bool supress);

		void AddLog(const Log& log);

		void StartListening();
		void StopListening();

		inline std::vector<Log> GetLogs() const { return logs; }
		inline bool DoesSupress() const { return supress; }

		LogListener& operator=(const LogListener&) = delete;
	private:
		bool supress;
		std::vector<Log> logs;
	};
}