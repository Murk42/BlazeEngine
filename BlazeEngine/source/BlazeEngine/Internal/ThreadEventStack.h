#pragma once
#include "BlazeEngine/Event/EventDispatcher.h"

namespace Blaze
{
	template<uint MaxStackSize>
	class EventStack
	{
		template<typename Callable, typename ... Args>
		struct EventInfo
		{
			Callable callable;
			std::tuple<Args...> args;

			EventInfo(Callable&& callable, std::tuple<Args...>&& args)
				: callable(std::forward<Callable>(callable)), args(std::move(args))
			{
			}
		};

		struct EventHeader
		{
			uint8 size;			
			void (*func)(void*);

			EventHeader(uint8 size, void (*func)(void*))
				: size(size), func(func)
			{

			}
		};

		template<typename Callable, typename ... Args>
		static void CallEventFromStack(void* ptr)
		{
			EventInfo<Callable, Args...>* eventInfo = (EventInfo<Callable, Args...>*)ptr;


			std::apply(eventInfo->callable, eventInfo->args);			
		}

		template<typename Callable, typename ... Args>
		struct EventInstance
		{
			EventHeader header;
			EventInfo<Callable, Args...> info;

			EventInstance(Callable&& callable, std::tuple<Args...>&& args) :
				header(sizeof(EventInstance<Callable, Args...>), CallEventFromStack<Callable, Args...>),
				info(std::forward<Callable>(callable), std::move(args))
			{			
			}
		};

		std::mutex mutex;
		uint8 stack[MaxStackSize];
		uint stackSize = 0;
		std::condition_variable cv;
	public:
		void ProcessAndClear()
		{
			/*
				Implementation details:

				We try to execute all available events without locking the mutex because we will be running client
				code so we dont know what can happen. So we store the stack size and use it until that point, later
				we lock the mutex and move the rest of the stack (if there is more at that point) to the begging without processing it.
			*/

			uint oldStackSize = 0;

			{
				std::lock_guard<std::mutex> lk{ mutex };
				oldStackSize = stackSize;
			}

			void* ptr = stack;
			void* end = stack + oldStackSize;

			while (ptr != end)
			{
				EventHeader* header = (EventHeader*)ptr;
				header->func((char*)ptr + sizeof(EventHeader));
				ptr = (char*)ptr + header->size;
			}

			{
				std::lock_guard<std::mutex> lk{ mutex };

				uint newStackSize = stackSize - oldStackSize;
				memmove_s(stack, MaxStackSize, stack + oldStackSize, newStackSize);
				stackSize = newStackSize;
			}

			cv.notify_all();
		}
		void WaitUntilEmpty()
		{
			std::unique_lock<std::mutex> lk{ mutex };
			cv.wait(lk, [&]() { return stackSize == 0; });
		}

		template<typename ... Args, typename Callable> requires std::invocable<Callable, Args...>
		void Add(StringView eventName, Callable&& callable, const Args& ... args)
		{
			constexpr uint size = sizeof(EventInstance<Callable, Args...>);
			
			std::unique_lock<std::mutex> lk{ mutex };

			if (!cv.wait_for(lk, std::chrono::seconds(1), [&]() { return MaxStackSize >= stackSize + size; }))
			{				
				Debug::Logger::LogWarning("Blaze Engine", "Input event stack overflow. The receiving thread didn't process any events for more than 1 second. Ignoring event named \"" + eventName + "\"");
				return;
			}

			EventInstance<Callable, Args...>* instance = std::construct_at((EventInstance<Callable, Args...>*)(stack + stackSize), std::forward<Callable>(callable), std::tuple<Args...>(args...));
			stackSize += instance->header.size;
		}
	};	
}