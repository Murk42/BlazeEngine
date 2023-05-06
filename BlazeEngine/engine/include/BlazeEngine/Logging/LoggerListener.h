#pragma once
#include <utility>
#include <functional>

namespace Blaze
{
	class BLAZE_API LoggerListener
	{
	public:
		LoggerListener();		
		LoggerListener(bool supress, bool startListening);
		~LoggerListener();						

		void StartSupressing() { supress = true; }
		void StopSupressing() { supress = false; }
		void StartListening();
		void StopListening();

		bool IsSupressing() const { return supress; }
		bool IsListening() const { return listening; }
		
		void Add(Result&& result);
		void Add(Log log);
		
		LoggerListener& operator+= (Result&& result);
		LoggerListener& operator+= (Log log);

		virtual void NewResult(Result&&) { }
		virtual bool NewLog(const Log&) { return true; }		

		Result ToResult();		
		const Result& GetResult() const { return result; }

		template<std::invocable<> F> 
		static Result&& Listen(const F& function);
		template<std::invocable<const Result&> F> 
		static Result&& Listen(const F& function);
	private:				
		bool supress;
		bool listening;		

		Result result;		
	};		

	template<std::invocable<> F>
	static Result&& LoggerListener::Listen(const F& function)
	{
		Result result;
		LoggerListener listener{ true, true };

		if constexpr (std::is_same_v<std::invoke_result_t<F>, Result>)
			result = function();
		else
			function();

		result += listener.ToResult();

		return std::move(result);
	}
	template<std::invocable<const Result&> F>
	static Result&& LoggerListener::Listen(const F& function)
	{
		Result result;
		LoggerListener listener{ true, true };

		if constexpr (std::is_same_v<std::invoke_result_t<F>, Result>)
			result = function(listener.GetResult());
		else
			function(listener.GetResult());

		result += listener.ToResult();

		return std::move(result);
	}
}