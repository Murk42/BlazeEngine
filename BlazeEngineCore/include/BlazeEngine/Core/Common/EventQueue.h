#pragma once
#include "BlazeEngine/Core/Container/Queue.h"
#include <mutex>
#include <condition_variable>

namespace Blaze
{
	template<typename T, AllocatorType Allocator = DefaultAllocator> requires IsAssignableFrom<T, const T&>
	class BLAZE_API EventQueue
	{
	public:
		EventQueue();
		~EventQueue();

		uintMem Count();

		template<typename ... Args> requires IsConstructibleFrom<T, Args...>
		void AddEvent(Args&& ... args);
		bool GetEvent(T& event);
		bool WaitEvent(float seconds, T& event);

		bool HasEvents() const;
	private:
		mutable std::mutex mutex;
		mutable std::condition_variable cv;
		Queue<T, Allocator> queue;
	};
	template<typename T, AllocatorType Allocator> requires IsAssignableFrom<T, const T&>
	inline EventQueue<T, Allocator>::EventQueue()
	{

	}
	template<typename T, AllocatorType Allocator> requires IsAssignableFrom<T, const T&>
	inline EventQueue<T, Allocator>::~EventQueue()
	{
	}
	template<typename T, AllocatorType Allocator> requires IsAssignableFrom<T, const T&>
	inline uintMem EventQueue<T, Allocator>::Count() 
	{
		std::lock_guard lock{ mutex };
		return queue.Count();
	}
	template<typename T, AllocatorType Allocator> requires IsAssignableFrom<T, const T&>
	template<typename ... Args> requires IsConstructibleFrom<T, Args...>
	inline void EventQueue<T, Allocator>::AddEvent(Args&& ... args)
	{
		std::lock_guard lock{ mutex } ;
		queue.Push(std::forward<Args>(args)...);

		cv.notify_one();
	}
	template<typename T, AllocatorType Allocator> requires IsAssignableFrom<T, const T&>
	inline bool EventQueue<T, Allocator>::GetEvent(T& event)
	{
		std::lock_guard lock{ mutex };

		if (queue.Empty())
			return false;

		event = queue.Pop();
		return true;
	}
	template<typename T, AllocatorType Allocator> requires IsAssignableFrom<T, const T&>
	inline bool EventQueue<T, Allocator>::WaitEvent(float seconds, T& event)
	{
		std::unique_lock lock{ mutex };

		if (cv.wait_for(lock, std::chrono::duration<float>(seconds), [this]() { return !queue.Empty(); }))
		{
			event = queue.Pop();
			return true;
		}

		return false;
	}
	template<typename T, AllocatorType Allocator> requires IsAssignableFrom<T, const T&>
	inline bool EventQueue<T, Allocator>::HasEvents() const
	{
		std::lock_guard lock{ mutex };
		return !queue.Empty();
	}
}