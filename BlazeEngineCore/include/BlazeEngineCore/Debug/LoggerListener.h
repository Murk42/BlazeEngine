#pragma once

namespace Blaze::Debug
{
	class BLAZE_CORE_API LoggerListener
	{
	public:		
		LoggerListener(bool supress = true, bool startListening = true);
		~LoggerListener();						

		void StartSupressing() { supress = true; }
		void StopSupressing() { supress = false; }
		void StartListening();
		void StopListening();

		bool IsSupressing() const { return supress; }
		bool IsListening() const { return listening; }						

		virtual void NewString(StringView) { }
		virtual void NewResult(const Result&) { }
		virtual void NewLog(const Debug::Log&) { }
		
		Result& GetResult() { return result; }		
	private:				
		bool supress;
		bool listening;

		Result result;
	};			
}