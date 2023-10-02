#pragma once
#include "BlazeEngine/Event/EventDispatcher.h"

namespace Blaze::Input
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
				std::lock_guard<std::mutex> lk { mutex};
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
				std::lock_guard<std::mutex> lk { mutex};
				
				uint newStackSize = stackSize - oldStackSize;
				memmove_s(stack, MaxStackSize, stack + oldStackSize, newStackSize);
				stackSize = newStackSize;
			}
		}
		template<typename T>
		void Add(const T& event, EventDispatcher<T>* eventDispatcher)
		{									
			std::lock_guard<std::mutex> lk { mutex};
			
			constexpr uint size = sizeof(EventInstance<T>);

			if (MaxStackSize < stackSize + size)
			{
				Debug::Logger::LogWarning("Blaze Engine", "Input event stack overflow. Ignoring event of type \"" + String(typeid(T).name()) + "\"");
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
	
	extern EventStack<2048> clientEventStack;	
}