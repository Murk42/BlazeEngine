#pragma once

namespace Blaze::UI
{
	class InputManager;
	class Node;

	class BLAZE_GRAPHICS_API UIMouseEventHandler
	{
	public:		
		struct MouseButtonDownEvent  
		{ 
			InputManager&inputManager;
			Node& node;
			uint64 timeNS;
			Mouse::MouseID mouseID;

			Mouse::MouseButton button;
			uint combo;

			Vec2f screenPos;
			Vec2f windowPos;
			Vec2f desktopPos;
		};
		struct MouseButtonUpEvent 
		{ 
			InputManager& inputManager; 			
			Node& node;
			uint64 timeNS;
			Mouse::MouseID mouseID;

			Mouse::MouseButton button;
			uint combo;

			Vec2f screenPos;
			Vec2f windowPos;
			Vec2f desktopPos;
		};
		struct MouseEnterEvent    
		{ 
			InputManager& inputManager; 			
			Node& node;
			uint64 timeNS;
		};
		struct MouseExitEvent     
		{ 
			InputManager& inputManager; 			
			Node& node;
			uint64 timeNS;
		};
		struct MouseMotionEvent   
		{ 
			InputManager& inputManager; 
			Node& node;
			uint64 timeNS;
			Mouse::MouseID mouseID;

			Vec2f delta;  //In window pixel units
			Vec2f screenPos;
			Vec2f windowPos;
			Vec2f desktopPos;
		};
		struct MouseScrollEvent   
		{ 
			InputManager& inputManager; 
			Node& node;
			uint64 timeNS;
			Mouse::MouseID mouseID;

			Vec2f value;
			Mouse::ScrollDirection direction;

			Vec2f screenPos;
			Vec2f windowPos;
			Vec2f desktopPos;
		};

		virtual void OnEvent(const MouseMotionEvent& event) {  }
		virtual void OnEvent(const MouseScrollEvent& event) {  }
		virtual void OnEvent(const MouseButtonDownEvent& event) {  }
		virtual void OnEvent(const MouseButtonUpEvent& event) {  }
		virtual void OnEvent(const MouseEnterEvent& event) { }
		virtual void OnEvent(const MouseExitEvent& event) { }	
	private:		
		friend class InputManager;
	};
}