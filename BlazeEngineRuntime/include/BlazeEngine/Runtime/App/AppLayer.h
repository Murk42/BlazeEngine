#pragma once
#include "BlazeEngine/Runtime/IO/Window.h"

namespace Blaze
{
	class AppLayer
	{
	public:
		struct DestructionEvent
		{
			AppLayer& layer;
		};
		EventDispatcher<DestructionEvent> destructionEventDispatcher;

		AppLayer();
		virtual ~AppLayer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void Update() {}
		virtual void Render() {}

		virtual bool OnEvent(const Input::GenericInputEvent& event, bool isProcessed) { return false; }

		void ScheduleForDestruction();
		bool IsScheduledForDesturction() const;
	private:
		bool scheduledForDestruction;
	};
}