#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Debug/Log.h"

namespace Blaze
{
	class BLAZE_API Result
	{
	public:
		Result();
		Result(const Result&);
		Result(Result&&) noexcept;
		Result(const Debug::Log& log);
		Result(std::initializer_list<Debug::Log> logs);

		~Result();

		void AddLogs(std::initializer_list<Debug::Log> logs);
		void AddLog(const Debug::Log& log);

		void Clear();
		void ClearSilent();

		bool IsEmpty() const;
		Debug::LogType HighestLogType() const { return highestLogType; }

		inline ArrayView<Debug::Log> GetLogs() const { return logs; }

		operator bool() const;

		Result& JoinResults(Result& result) noexcept;
		Result& JoinResults(Result&& result) noexcept;

		Result& operator+=(Result&) noexcept;
		Result& operator+=(Result&&) noexcept;
		Result& operator+=(Debug::Log);

		Result& operator=(const Result&);
		Result& operator=(Result&&) noexcept;
	private:
		Array<Debug::Log, DefaultAllocator> logs;
		Debug::LogType highestLogType;

	};

	BLAZE_API Result operator+(Result& a, Result&& b);
	BLAZE_API Result operator+(Result&& a, Result& b);
	BLAZE_API Result operator+(Result&& a, Result&& b);
}