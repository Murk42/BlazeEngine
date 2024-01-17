#pragma once
#include "BlazeEngine/Event/EventDispatcher.h"

namespace Blaze
{
	template<uint MaxStackSize>
	class EventStack
	{
		template<typename T>
		struct EventInfo
		{
			EventDispatcher<T>* dispatcher;
			T event;
		};

		struct EventHeader
		{
			uint8 size;
			void(*fnc)(void*);
		};

		template<typename T>
		struct EventInstance
		{
			EventHeader header;
			EventInfo<T> info;
		};
		template<typename T>
		static void CallEventFromStack(void* ptr)
		{
			EventInfo<T>* eventInfo = (EventInfo<T>*)ptr;

			eventInfo->dispatcher->Call(eventInfo->event);
			eventInfo->event.~T();
		}

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
				header->fnc((char*)ptr + sizeof(EventHeader));
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

		template<typename T>
		void Add(const T& event, EventDispatcher<T>* eventDispatcher)
		{
			constexpr uint size = sizeof(EventInstance<T>);

			std::unique_lock<std::mutex> lk{ mutex };

			if (!cv.wait_for(lk, std::chrono::seconds(1), [&]() { return MaxStackSize >= stackSize + size; }))
			{
				const char* str = typeid(T).name();
				uintMem len = strlen(str);
				Debug::Logger::LogWarning("Blaze Engine", "Input event stack overflow. The receiving thread didn't process any events for more than 1 second. Ignoring event of type \"" + String(str, len) + "\"");
				return;
			}

			EventInstance<T>* instance = (EventInstance<T>*)(stack + stackSize);
			instance->header.size = sizeof(EventInstance<T>);
			instance->header.fnc = CallEventFromStack<T>;
			instance->info.dispatcher = eventDispatcher;
			std::construct_at(&instance->info.event, event);

			stackSize += size;
		}
	};	
}