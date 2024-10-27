#include "pch.h"
#include "BlazeEngineCore/Debug/Result.h"

namespace Blaze
{	
	Result::Result() :
		highestLogType(Debug::LogType::Info)
	{						
	}
	Result::Result(const Result& result) : 
		logs(result.logs), highestLogType(result.highestLogType)
	{		
	}

	Result::Result(Result&& result) noexcept : 
		logs(std::move(result.logs)), highestLogType(result.highestLogType)
	{		
		result.highestLogType = Debug::LogType::Info;
	}
	Result::Result(const Debug::Log& log) :
		highestLogType(Debug::LogType::Info)
	{		
		AddLog(log);
	}
	Result::Result(std::initializer_list<Debug::Log> logs) :
		highestLogType(Debug::LogType::Info)
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
	void Result::AddLog(const Debug::Log& log)
	{
		logs.AddBack(log);		
	}	

	StringUTF8 Result::ToString() const
	{		
		StringUTF8 out;
		
		if (IsEmpty())
		{
			out = "Empty result";
		}
		else
		{			
			for (uintMem i = 0; i < logs.Count() - 1; ++i)				
				out += logs[i].ToString() + "\n";				
			out += logs.Last().ToString();
		}

		return out;		
	}

	StringUTF8 Result::ToStringVerbose() const
	{
		StringUTF8 out;

		if (IsEmpty())
		{
			out = "Empty result";
		}
		else
		{
			out += "-------------------------------------------------\n";

			for (auto& log : logs)			
				if (log.GetType() >= Debug::LogType::Error)
					out += log.ToStringVerbose() + "\n";
				else
					out += log.ToString() + "\n";

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
		logs.Clear();
		highestLogType = Debug::LogType::Info;
	}

	bool Result::IsEmpty() const
	{
		return logs.Empty();
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

		logs.Append(std::move(result.logs));
		highestLogType = std::max(highestLogType, result.highestLogType);		

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

		logs = other.logs;
		highestLogType = other.highestLogType;
				
		return *this;
	}
	Result& Result::operator=(Result&& other) noexcept	
	{		
		if (this == &other)
			return *this;
		
		logs = std::move(other.logs);
		highestLogType = other.highestLogType;
		other.highestLogType = Debug::LogType::Info;

		return *this;
	}			

	Result operator+(Result& a, Result&& b)
	{
		return std::move(a += b);		
	}

	BLAZE_CORE_API Result operator+(Result&& a, Result& b)
	{						
		return std::move(a += b);
	}

	BLAZE_CORE_API Result operator+(Result&& a, Result&& b)
	{
		return std::move(a += b  );
	}
}