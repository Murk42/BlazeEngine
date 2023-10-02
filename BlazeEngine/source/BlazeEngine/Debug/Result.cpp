#include "pch.h"
#include "BlazeEngine/Debug/Result.h"

namespace Blaze
{
	Result::Result()	
		: logs(nullptr), logCount(0), logType(Debug::LogType::Info), supressFatalsLogs(false)
	{				
	}
	Result::Result(const Result& result)
		: logs(nullptr), logCount(0), logType(Debug::LogType::Info), supressFatalsLogs(result.supressFatalsLogs)
	{
		CopyUnsafe(result);
	}

	Result::Result(Result&& result) noexcept
		: logs(result.logs), logCount(result.logCount), logType(result.logType), supressFatalsLogs(result.supressFatalsLogs)
	{
		result.logs = nullptr;
		result.logCount = 0;
	}
	Result::Result(Debug::Log log)
		: logs(nullptr), logCount(0), logType(log.GetType()), supressFatalsLogs(false)
	{		
		AddLog(std::move(log));		
	}
	Result::Result(std::initializer_list<Debug::Log> logs)
		: logType(Debug::LogType::Info), supressFatalsLogs(false)
	{
		AddLogs(logs);
	}

	Result::~Result()
	{		
		Clear();
	}
	void Result::AddLogs(std::initializer_list<Debug::Log> logs)
	{		
		for (auto& log : logs)		
			AddLog(log);
	}
	void Result::AddLog(Debug::Log log)
	{
		AppendUnsafe(&log, 1);
	}	

	String Result::ToString() const
	{		
		String out;
		
		if (IsEmpty())
		{
			out = "Empty result";
		}
		else
		{
			out += "-------------------------------------------------\n";

			for (auto it = logs; it != logs + logCount; ++it)			
				out += it->ToString() + "\n";			

			out += "-------------------------------------------------\n";
		}

		return out;		
	}

	String Result::ToStringVerbose() const
	{
		String out;

		if (IsEmpty())
		{
			out = "Empty result";
		}
		else
		{
			out += "-------------------------------------------------\n";

			for (auto it = logs; it != logs + logCount; ++it)
				if (it->GetType() >= Debug::LogType::Error)
					out += it->ToStringVerbose() + "\n";
				else
					out += it->ToString() + "\n";

			out += "-------------------------------------------------\n";
		}

		return out;
	}

	void Result::Clear()
	{		
		if (!IsEmpty())
			Debug::Logger::ProcessResult(std::move(*this));

		ClearSilent();
	}

	void Result::ClearSilent()
	{
		std::destroy_n(logs, logCount);
		Memory::Free(logs);
		logs = nullptr;
		logCount = 0;
	}

	bool Result::IsEmpty() const
	{
		return logCount == 0;
	}

	void Result::SupressFatalLogs()
	{
		this->supressFatalsLogs = true;
	}

	Result::operator bool() const
	{
		return !IsEmpty();
	}

	Result& Result::JoinResults(Result& result) noexcept
	{
		return JoinResults(std::move(result));		
	}

	Result& Result::JoinResults(Result&& result) noexcept
	{
		if (result.IsEmpty())
			return *this;
		
		AppendUnsafe(result.logs, result.logCount);
		result.logs = nullptr;
		result.logCount = 0;
		result.logType = Debug::LogType::Info;		

		return *this;		
	}

	Result& Result::operator+=(Debug::Log log)
	{
		AddLog(std::move(log));

		return *this;
	}
	Result& Result::operator+=(Result& result) noexcept
	{
		return JoinResults(result);
	}
	Result& Result::operator+=(Result&& result) noexcept
	{
		return JoinResults(result);
	}
	
	Result& Result::operator=(const Result& other)
	{
		if (this == &other)
			return *this;		

		Clear();
		CopyUnsafe(other);		
		return *this;
	}
	Result& Result::operator=(Result&& result) noexcept	
	{		
		if (this == &result)
			return *this;

		if (!IsEmpty())
			Debug::Logger::ProcessResult(std::move(*this)); 

		logType = result.logType;
		logs = std::move(result.logs);		
		return *this;
	}	
		
	void Result::CopyUnsafe(const Result& other)
	{		
		logs = (Debug::Log*)Memory::Allocate(sizeof(Debug::Log) * other.logCount);
		logCount = other.logCount;
		logType = other.logType;		

		for (uint i = 0; i < logCount; ++i)
			std::construct_at(&logs[i], other.logs[i]);
	}

	void Result::AppendUnsafe(Debug::Log* newLogs, uint count)
	{
		Debug::Log* old = logs;
		logs = (Debug::Log*)Memory::Allocate(sizeof(Debug::Log) * (logCount + count));

		for (uint i = 0; i < logCount; ++i)
			std::construct_at(&logs[i], std::move(old[i]));

		for (uint i = 0; i < count; ++i)
		{
			std::construct_at(&logs[i + logCount], std::move(newLogs[i]));
			if ((uint)newLogs[i].type > (uint)logType)
				logType = newLogs[i].type;
		}

		Memory::Free(old);
		logCount += count;
	}

	Result operator+(Result& a, Result&& b)
	{
		return std::move(a += b);		
	}

	BLAZE_API Result operator+(Result&& a, Result& b)
	{						
		return std::move(a += b);
	}

	BLAZE_API Result operator+(Result&& a, Result&& b)
	{
		return std::move(a += b  );
	}
}