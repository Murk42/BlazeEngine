#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineCore/Event/EventDispatcher.h"
#include "BlazeEngineGraphics/UI/Input/InputNode.h"
#include "BlazeEngineGraphics/UI/Input/MouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/SelectEventHandler.h"

namespace Blaze::UI
{
	class BLAZE_GRAPHICS_API ButtonBase :
		public InputNode,
		public UIMouseEventHandler,
		public UISelectEventHandler
	{
	public:
		struct PressableFlagChangedEvent
		{
			ButtonBase& button;
		};
		struct PressedEvent 
		{
			InputManager& inputManager; 
			ButtonBase& button;
		};
		EventDispatcher<PressableFlagChangedEvent> pressableFlagChangedEventDispatcher;
		std::function<void(PressedEvent)> pressedEventCallback;		

		ButtonBase();

		void SetPressableFlag(bool pressable);

		inline bool IsPressed() const { return pressed; }	
		inline bool IsPressable() const { return pressable; }
	protected:		
		void OnEvent(const MouseButtonDownEvent& event) override;
		void OnEvent(const MouseButtonUpEvent& event) override;		
		void OnEvent(const DeselectedEvent& event) override;
	private: 				
		bool pressed : 1;		
		bool pressable : 1;
	};
}