#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{
	class Result;
}

namespace Blaze::Debug
{
	class BLAZE_CORE_API LoggerListener
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
		
		virtual void NewResult(const Result&) { }					
	private:				
		bool suppress;
		bool listening;
	};			
}