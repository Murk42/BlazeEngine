#include "BlazeEngine/Logger/LogListener.h"

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
	void LogListener::StopListening()
	{
		RemoveLogListener(this);
	}
	void LogListener::StartListening()
	{ 
		AddLogListener(this);
	}
}