#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include <thread>
#include <mutex>
#include <functional>

namespace Blaze
{
	namespace Threading
	{		
		class Mutex
		{
			std::mutex m;
		public:
			inline Mutex()
				: m()
			{
			}
			Mutex(const Mutex&) = delete;
			
			inline void Lock()
			{
				m.lock();
			}
			inline bool TryLock()
			{
				return m.try_lock();
			}
			inline void Unlock()
			{
				m.unlock();
			}

			Mutex& operator=(const Mutex&) = delete;

			friend class LockGuard;
		};

		class LockGuard
		{
			std::lock_guard<std::mutex> lock;
		public:
			inline LockGuard(Mutex& mutex)
				: lock(mutex.m)
			{

			}
			LockGuard(const LockGuard&) = delete;			

			LockGuard& operator=(const LockGuard&) = delete;
		};

		class Thread
		{
			std::thread t;
		public:			
			inline Thread()
				: t()
			{

			}
			inline Thread(const Thread&) = delete;
			inline Thread(Thread&& thread) noexcept 
				: t(std::move(thread.t))
			{

			}
			template<typename R, typename ... A>
			inline Thread(std::function<R(A...)>&& function, A&& ... args)
				: t(std::move(function), std::move(args)...)
			{
			}

			inline void Join()
			{
				t.join();
			}
			inline bool Joinable() const
			{
				return t.joinable();
			}

			inline void Detach()
			{
				t.detach();
			}

			inline Thread& operator=(const Thread&) = delete;
			inline Thread& operator=(Thread&& thread) noexcept
			{
				t.operator=(std::move(thread.t));
				return *this;
			}
		};		
	}
}