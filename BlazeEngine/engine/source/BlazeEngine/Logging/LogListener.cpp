#include "BlazeEngine/Logging/LogListener.h"

namespace Blaze
{
	void AddLogListener(LogListener* listener);
	void RemoveLogListener(LogListener* listener);

	LogListener::LogListener()
		: supress(false)
	{
	}
	LogListener::~LogListener()
	{
	}

	void LogListener::SupressLogs(bool supress)
	{
		this->supress = supress;
	}
	void LogListener::AddLog(const Log& log)
	{
		logs.emplace_back(log);
	}
	void LogListener::StartListening()
	{ 
		AddLogListener(this);
	}
	void LogListener::StopListening()
	{
		RemoveLogListener(this);
	}
	void LogListener::PushLogs()
	{
		for (auto log : logs)
			Logger::AddLog(log);
		logs.clear();
	}
	void LogListener::ClearLogs()
	{
		logs.clear();
	}
}