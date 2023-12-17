#include "pch.h"
#include "BlazeEngineCore/Debug/LoggerListener.h"

namespace Blaze::Debug
{
	namespace Logger
	{
		void AddLoggerListener(LoggerListener* listener);
		void RemoveLoggerListener(LoggerListener* listener);
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
			Logger::AddLoggerListener(this);
		listening = true;
	}

	void LoggerListener::StopListening()
	{
		if (listening)
			Logger::RemoveLoggerListener(this);
		listening = false;
	}		
}