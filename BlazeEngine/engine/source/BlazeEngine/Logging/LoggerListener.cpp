#include "BlazeEngine/Logging/LoggerListener.h"

namespace Blaze
{
	void AddLoggerListener(LoggerListener* listener);
	void RemoveLoggerListener(LoggerListener* listener);

	LoggerListener::LoggerListener()
		: supress(false), listening(false)
	{
	}
	LoggerListener::LoggerListener(bool supress, bool startListening)
		: supress(supress), listening(false)
	{
		if (startListening)
			StartListening();
	}
	LoggerListener::~LoggerListener()
	{
		if (listening)
			StopListening();			
	}

	void LoggerListener::StartListening()
	{
		if (!listening)
			AddLoggerListener(this);
		listening = true;
	}

	void LoggerListener::StopListening()
	{
		if (listening)
			RemoveLoggerListener(this);
		listening = false;
	}

	
	void LoggerListener::Add(Result&& result)
	{		
		if (listening)
		{
			NewResult(std::move(result));

			if (!result.IsEmpty())
				this->result += std::move(result);
		}
	}

	void LoggerListener::Add(Log log)
	{
		if (listening)
		{
			if (NewLog(log))
				this->result += std::move(log);
		}
	}

	LoggerListener& LoggerListener::operator+=(Result&& result)
	{
		Add(std::move(result));
		return *this;
	}

	LoggerListener& LoggerListener::operator+=(Log log)
	{
		Add(std::move(log));
		return *this;
	}

	Result LoggerListener::ToResult()
	{		
		return std::move(result);
	}
}