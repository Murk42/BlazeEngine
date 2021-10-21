#pragma once
#include "BlazeEngine/Core/EngineCore.h"	

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace Blaze
{
	class WaitPoint
	{
		std::mutex m;
		std::condition_variable cv;
		bool waiting = false;
	public:
		template<typename T>
		void Wait(const T& f)
		{
			std::unique_lock<std::mutex> lk(m);

			if (waiting)
			{
				cv.notify_one();
				f();
			}
			else
			{
				waiting = true;
				cv.wait(lk);
				f();
				waiting = false;
			}
		}

		void Wait()
		{
			std::unique_lock<std::mutex> lk(m);

			if (waiting)
				cv.notify_one();
			else
			{
				waiting = true;
				cv.wait(lk);
				waiting = false;
			}
		}
	};	

	template<typename T>
	class SharedVariable
	{
		std::mutex m;
		T value;
	public:
		inline bool TryRead(T& out)
		{
			if (m.try_lock())
			{
				out = value;
				m.unlock();
				return true;
			}
			return false;
		}
		inline void Read(T& out)
		{
			m.lock();
			out = value;
			m.unlock();
		}
		inline void TryWrite(const T& in)
		{
			if (m.try_lock())
			{
				value = in;
				m.unlock();
				return true;
			}
			return false;
		}
		inline void Write(const T& in)
		{
			m.lock();
			value = in;
			m.unlock();
		}
	};

	template<typename R, typename ... A>
	class Worker
	{
		template<typename R>
		class ReturnStorage
		{
		public:
			R value;
			template<typename ... T>
			void SetValue(std::function<R(T...)> function, std::tuple<T...> arguments)
			{
				value = std::apply(function, arguments);
			}
			R GetValue() const { return value; }
		};

		template<>
		class ReturnStorage<void>
		{
		public:
			template<typename ... T>
			void SetValue(std::function<void(T...)> function, std::tuple<T...> arguments) { std::apply(function, arguments); }
			void GetValue() const { return; }
		};

		std::mutex m;
		std::function<R(A...)> function;
		std::tuple<A...> arguments;
		ReturnStorage<R> returned;

		WaitPoint waitPoint;
		std::thread t;
		bool exit;

		void ThreadFunction()
		{
			while (1)
			{
				std::unique_lock<std::mutex> lk(m, std::defer_lock);
				waitPoint.Wait([&]() {lk.lock(); });

				if (exit)
					return;
				else
					returned.SetValue(function, arguments);
			}
		}

		Worker(const Worker<R, A...>&) = delete;
		void operator=(const Worker<R, A...>&) = delete;
	public:
		Worker()
			: exit(false)
		{
			t = std::thread([this]() { this->ThreadFunction(); });
		}
		Worker(std::function<R(A...)> function, const A& ... arguments)
			: exit(false), arguments(std::make_tuple(arguments...))
		{
			t = std::thread([this]() { this->ThreadFunction(); });
			AssignWork(function, arguments...);
		}		

		~Worker()
		{
			{
				std::lock_guard<std::mutex> lk(m);
				exit = true;
			}
			waitPoint.Wait();
			t.join();
		}

		bool IsFinished()
		{
			if (m.try_lock())
			{
				m.unlock();
				return true;
			}
			return false;
		}
		void WaitToFinish()
		{
			volatile std::lock_guard<std::mutex> lk(m);
		}

		void AssignWork(std::function<R(A...)> function, A ... arguments)
		{
			{
				std::lock_guard<std::mutex> lk(m);
				this->function = function;
				this->arguments = std::make_tuple(arguments...);
			}
			waitPoint.Wait();
		}

		R GetReturnedValue()
		{
			WaitToFinish();
			return returned.GetValue();
		}

	};
}