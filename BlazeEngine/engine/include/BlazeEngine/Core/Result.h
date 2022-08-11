#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/Logging/Log.h"
#include <vector>

namespace Blaze
{
	class BLAZE_API Result
	{
	public:
		Result();
		Result(const Result&) = delete;
		Result(Result&&) noexcept;

		Result(Result&);
		Result(const Log& log, bool failed = true);
		Result(Result& result, const Log& log, bool failed = true);

		~Result();		

		void AddLogs(const std::vector<Log>& logs);
		void AddLog(const Log& log);

		void SetFailed(bool failed);
		std::vector<Log> GetLogs() const { return logs; }
		void Clear();

		operator bool() const { return failed; }

		Result& operator=(const Result&) = delete;
		Result& operator=(Result&&) noexcept;
	private:
		bool failed;
		LogType type;
		std::vector<Log> logs;
	};
}