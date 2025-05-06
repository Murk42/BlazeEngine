#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BLazeEngineCore/Debug/Log.h"
#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/DataStructures/ArrayView.h"

namespace Blaze
{	
	class BLAZE_CORE_API Result
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

		StringUTF8 ToString() const;
		StringUTF8 ToStringVerbose() const;

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
		Array<Debug::Log> logs;		
		Debug::LogType highestLogType;		

	};

	BLAZE_CORE_API Result operator+(Result& a, Result&& b);	
	BLAZE_CORE_API Result operator+(Result&& a, Result& b);
	BLAZE_CORE_API Result operator+(Result&& a, Result&& b);
}