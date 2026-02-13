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

		virtual Input::EventProcessedState OnEvent(const Input::GenericInputEvent& event, bool processed) { return Input::EventProcessedState::NotProcessed; }

		void ScheduleForDestruction();
		bool IsScheduledForDesturction() const;
	private:
		bool scheduledForDestruction;
	};
}