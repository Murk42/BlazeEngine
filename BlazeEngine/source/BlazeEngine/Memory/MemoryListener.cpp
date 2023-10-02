#include "BlazeEngine/Memory/MemoryListener.h"

namespace Blaze
{
	namespace Memory
	{
		void AddListener(MemoryListener* listener);
		void RemoveListener(MemoryListener* listener);
		void ChangeListener(MemoryListener* old, MemoryListener* ptr);
	}

	MemoryListener::MemoryListener()
		: listening(false)
	{

	}	
	MemoryListener::MemoryListener(MemoryListener&& o) noexcept
		: events(std::move(o.events)), listening(o.listening)
	{		
		if (o.listening)
			Memory::ChangeListener(&o, this);
	}
	MemoryListener::~MemoryListener()
	{

	}
	void MemoryListener::AddMemoryEvent(const MemoryEvent& event)
	{
		events.AddBack(event);
	}
	Result MemoryListener::StartListening()
	{
		if (listening)
			return BLAZE_WARNING_RESULT("Blaze Engine", "The moemory listener is already listening");

		Memory::AddListener(this);
		listening = true;
		return { };
	}
	Result MemoryListener::StopListening()
	{
		if (!listening)
			return BLAZE_WARNING_RESULT("Blaze Engine", "The moemory listener isnt listening anymore");

		Memory::RemoveListener(this);
		listening = false;
		return { };
	}

	void MemoryListener::ClearEvents()
	{
		events.Clear();
	}	
	
	MemoryListener& MemoryListener::operator=(MemoryListener&& o) noexcept
	{		
		events = std::move(o.events);
		listening = o.listening;
		if (o.listening)
			Memory::ChangeListener(&o, this);
		return *this;
	}
}