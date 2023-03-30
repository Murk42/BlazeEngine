#pragma once
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/Console/Console.h"

namespace Blaze
{
	Result::Result()
		: failed(false)
	{		
	}	
	Result::Result(Result&& result) noexcept
		: logs(std::move(result.logs)), failed(result.failed)
	{
		result.Clear();
	}
	Result::Result(const Log& log, bool failed)
		: Result()
	{		
		this->failed = failed;
		AddLog(log);
	}	
	Result::~Result()
	{
		for (auto& log : logs)
			Logger::AddLog(log);
	}
	void Result::AddLogs(const std::vector<Log>& logs)
	{
		for (auto& log : logs)
			AddLog(log);		
	}
	void Result::AddLog(const Log& log)
	{
		logs.emplace_back(log);		
	}
	void Result::SetFailed(bool failed)
	{
		this->failed = failed;
	}
	void Result::Clear()
	{
		logs.clear();		
		failed = false;
	}

	String Result::FormatString() const
	{
		String text;		
		for (auto l : logs)
			text += l.FormatString() + "\n";
		return text;
	}
	
	Result& Result::operator=(Result&& result) noexcept
	{		
		logs = std::move(result.logs);
		failed = result.failed;				

		result.Clear();

		return *this;
	}	
	Result Result::operator+(Result&& other)
	{
		Result out = std::move(*this);
		out.failed |= other.failed;		
		out.logs.insert(out.logs.end(), other.logs.begin(), other.logs.end());
		other.failed = false;
		
		return std::move(out);
	}
	Result Result::operator+(Result& other)
	{
		return *this + std::move(other);
	}
	Result& Result::operator+=(Result&& other)
	{
		return *this = std::move(*this + std::move(other));
	}
	Result& Result::operator+=(Result& other)
	{
		return operator+=(std::move(other));
	}
}