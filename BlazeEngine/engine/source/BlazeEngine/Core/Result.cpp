#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Logger/Logger.h"

namespace Blaze
{
	Result::Result()
		: type(LogType::Info), failed(false)
	{		
	}
	Result::Result(Result& result)
		: type(result.type), logs(result.logs), failed(result.failed)
	{
		result.Clear();
	}
	Result::Result(Result&& result) noexcept
		: type(result.type), logs(std::move(result.logs)), failed(result.failed)
	{
		result.Clear();
	}
	Result::Result(const Log& log, bool failed)
		: Result()
	{		
		this->failed = failed;
		AddLog(log);
	}
	Result::Result(Result& result, const Log& log, bool failed)
		: Result(result)
	{
		if (failed)
			this->failed = failed;

		AddLog(log);

		result.Clear();
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
		if (type < log.GetType())
			this->type = log.GetType();
	}
	void Result::SetFailed(bool failed)
	{
		this->failed = failed;
	}
	void Result::Clear()
	{
		logs.clear();
		type = LogType::Info;
		failed = false;
	}
	
	Result& Result::operator=(Result&& result) noexcept
	{
		type = result.type;
		logs = std::move(result.logs);
		failed = result.failed;				

		result.Clear();

		return *this;
	}
}