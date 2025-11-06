#pragma once
#include "BlazeEngine/Core/Event/EventDispatcher.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include <mutex>

namespace Blaze
{
	template<uint MaxStackSize>
	class EventStack
	{
	public:
		void ProcessEvents()
		{
			/*
				Implementation details:

				We copy the whole stack under the mutex, and then process all the events. It's important to process
				the events without locking the mutex to prevent a dead-lock, because user functions are being run.
			*/

			uint oldStackSize = 0;
			uint8 oldStack[MaxStackSize]{ };

			MoveEvents(oldStack, oldStackSize);
			ProcessEvents(oldStack, oldStackSize);
		}
		void WaitForAndProcessEvents()
		{
			std::unique_lock<std::mutex> lk{ mutex };

			cv.wait(lk, [&]() { return stackSize > 0; });

			uint oldStackSize = 0;
			uint8 oldStack[MaxStackSize]{ };
			MoveEvents(oldStack, oldStackSize);
			ProcessEvents(oldStack, oldStackSize);
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
				BLAZE_LOG_WARNING("Input event stack overflow. The receiving thread didn't process any events for more than 1 second. Ignoring event named \"" + eventName + "\"");
				return;
			}

			EventInstance<Callable, Args...>* instance = std::construct_at((EventInstance<Callable, Args...>*)(stack + stackSize), std::forward<Callable>(callable), std::tuple<Args...>(args...));
			stackSize += instance->header.size;
			cv.notify_all();
		}
		template<typename T>
		void Add(const T& event, EventDispatcher<T>* dispatcher)
		{
			const char* name = typeid(T).name();
			uintMem length = strlen(name);
			Add<T>(StringView(name, length), [](const T& event, EventDispatcher<T>* dispatcher) {
				dispatcher->Call(event);
				}, event, dispatcher);
		}
	private:
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

		void MoveEvents(uint8* outStack, uint& outStackSize)
		{
			std::lock_guard<std::mutex> lk{ mutex };
			outStackSize = stackSize;
			memcpy_s(outStack, MaxStackSize, stack, outStackSize);
			stackSize = 0;
			cv.notify_all();
		}
		static void ProcessEvents(void* stack, uintMem size)
		{
			void* ptr = stack;
			void* end = (char*)stack + size;

			while (ptr != end)
			{
				EventHeader* header = (EventHeader*)ptr;
				header->func((char*)ptr + sizeof(EventHeader));
				ptr = (char*)ptr + header->size;
			}
		}
	};
}