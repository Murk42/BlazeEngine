#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngineGraphics/UI/Input/UIInputNode.h"
#include "BlazeEngineGraphics/UI/Input/UIMouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UISelectEventHandler.h"

namespace Blaze::UI
{
	class BLAZE_GRAPHICS_API ButtonBase :
		public InputNode,
		public UIMouseEventHandler,
		public UISelectEventHandler
	{
	public:
		struct PressedEvent { InputManager* inputManager; };				

		EventDispatcher<PressedEvent> pressedEventDispatcher;

		ButtonBase();		

		inline bool IsPressed() const { return pressed; }	
	protected:
		void OnEvent(MousePressedEvent event) override;
		void OnEvent(MouseReleasedEvent event) override;		
		void OnEvent(DeselectedEvent event) override;
	private: 				
		bool pressed : 1;		

	};
}