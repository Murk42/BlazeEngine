#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

namespace Blaze
{
	class Result;
}

namespace Blaze::Debug
{
	class BLAZE_API LoggerListener
	{
	public:
		LoggerListener(bool suppress = true, bool startListening = true);
		~LoggerListener();

		void StartSuppressing() { suppress = true; }
		void StopSuppressing() { suppress = false; }
		void StartListening();
		void StopListening();

		bool IsSuppressing() const { return suppress; }
		bool IsListening() const { return listening; }

		virtual void NewLog(const Log&) { }
	private:
		bool suppress;
		bool listening;
	};
}