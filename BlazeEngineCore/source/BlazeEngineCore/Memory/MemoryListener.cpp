#include "pch.h"
#include "BlazeEngineCore/Memory/MemoryListener.h"

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
	void MemoryListener::StartListening()
	{
		if (listening)
			return;

		Memory::AddListener(this);
		listening = true;		
	}
	void MemoryListener::StopListening()
	{
		if (!listening)
			return;

		Memory::RemoveListener(this);
		listening = false;
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