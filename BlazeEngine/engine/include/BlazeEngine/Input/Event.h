#pragma once
#include "BlazeEngine/DataStructures/Common.h"
#include "Key.h"
#include <functional>

namespace Blaze
{
	class Window;

	enum class InputEventType
	{		
		KeyPressed,			
		KeyReleased,				

		MousePressed,       
		MouseReleased,      				
		MouseMotion,		
		MouseScroll,			
		MouseEnter,			
		MouseLeave,			

		WindowSizeChanged,	
		WindowResized,		
		WindowMoved,		
		WindowMinimized,	
		WindowMaximized,	
		WindowFocusGained,	
		WindowFocusLost,	
		WindowClosed,		

		InputEvent_Count
	};

	namespace InputEvent
	{
		struct KeyPressed			{ Key key; double time; uint count; };
		struct KeyReleased			{ Key key; double time; };		

		struct MousePressed			{ MouseKey key; Vec2i pos; double time; uint count; };
		struct MouseReleased		{ MouseKey key; Vec2i pos; double time; };      		
		struct MouseMotion			{ Vec2i pos; Vec2i delta; };
		struct MouseScroll			{ int value; };
		struct MouseEnter			{ };			
		struct MouseLeave			{ };			
	
		struct WindowSizeChanged	{ Vec2i size; Window* window; };	
		struct WindowResized		{ Vec2i size; Window* window; };		
		struct WindowMoved			{ Vec2i size; Window* window; };
		struct WindowMinimized		{ Window* window; };	
		struct WindowMaximized		{ Window* window; };	
		struct WindowFocusGained	{ Window* window; };	
		struct WindowFocusLost		{ Window* window; };	
		struct WindowClosed			{ Window* window; };		

		struct TextInput { String string; };
	}

	namespace InputEventFunction
	{
		using KeyPressed        = std::function<void(InputEvent::KeyPressed			)>;
		using KeyReleased       = std::function<void(InputEvent::KeyReleased		)>;		
		using MousePressed      = std::function<void(InputEvent::MousePressed		)>;
		using MouseReleased     = std::function<void(InputEvent::MouseReleased		)>;		
		using MouseMotion       = std::function<void(InputEvent::MouseMotion		)>;
		using MouseScroll       = std::function<void(InputEvent::MouseScroll		)>;
		using MouseEnter        = std::function<void(InputEvent::MouseEnter			)>;
		using MouseLeave        = std::function<void(InputEvent::MouseLeave			)>;		
		using WindowSizeChanged = std::function<void(InputEvent::WindowSizeChanged	)>;
		using WindowResized     = std::function<void(InputEvent::WindowResized		)>;
		using WindowMoved       = std::function<void(InputEvent::WindowMoved		)>;
		using WindowMinimized   = std::function<void(InputEvent::WindowMinimized	)>;
		using WindowMaximized   = std::function<void(InputEvent::WindowMaximized	)>;
		using WindowFocusGained = std::function<void(InputEvent::WindowFocusGained	)>;
		using WindowFocusLost   = std::function<void(InputEvent::WindowFocusLost	)>;
		using WindowClosed      = std::function<void(InputEvent::WindowClosed		)>;
		using TextInput			= std::function<void(InputEvent::TextInput			)>;
	}

	//template<InputEventType type> class InputEventFunction { };	
	//
	//template<> class InputEventFunction<InputEventType::KeyDown				> : public std::function<void(Key, double)			> { public: InputEventFunction(const std::function<void(Key, double)			>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::KeyPressed			> : public std::function<void(Key, double, uint)	> { public: InputEventFunction(const std::function<void(Key, double, uint)		>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::KeyReleased			> : public std::function<void(Key, double)			> { public: InputEventFunction(const std::function<void(Key, double)			>& func) : function(func) { } };
	//
	//template<> class InputEventFunction<InputEventType::MousePressed		> : public std::function<void(int dx, int dy)		> { public: InputEventFunction(const std::function<void(int dx, int dy)			>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::MouseReleased		> : public std::function<void(int dx, int dy)		> { public: InputEventFunction(const std::function<void(int dx, int dy)			>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::MouseDown			> : public std::function<void(int dx, int dy)		> { public: InputEventFunction(const std::function<void(int dx, int dy)			>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::MouseMotion			> : public std::function<void(int dx, int dy)		> { public: InputEventFunction(const std::function<void(int dx, int dy)			>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::MouseWheel			> : public std::function<void(int v)				> { public: InputEventFunction(const std::function<void(int v)					>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::MouseEnter			> : public std::function<void(Window&)				> { public: InputEventFunction(const std::function<void(Window&)				>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::MouseLeave			> : public std::function<void(Window&)				> { public: InputEventFunction(const std::function<void(Window&)				>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::WindowSizeChanged	> : public std::function<void(int w, int h, Window&)> { public: InputEventFunction(const std::function<void(int w, int h, Window&)	>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::WindowResized		> : public std::function<void(int w, int h, Window&)> { public: InputEventFunction(const std::function<void(int w, int h, Window&)	>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::WindowMoved			> : public std::function<void(int x, int y, Window&)> { public: InputEventFunction(const std::function<void(int x, int y, Window&)	>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::WindowMinimized		> : public std::function<void(Window&)				> { public: InputEventFunction(const std::function<void(Window&)				>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::WindowMaximized		> : public std::function<void(Window&)				> { public: InputEventFunction(const std::function<void(Window&)				>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::WindowFocusGained	> : public std::function<void(Window&)				> { public: InputEventFunction(const std::function<void(Window&)				>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::WindowFocusLost		> : public std::function<void(Window&)				> { public: InputEventFunction(const std::function<void(Window&)				>& func) : function(func) { } };
	//template<> class InputEventFunction<InputEventType::WindowClosed		> : public std::function<void(Window&)				> { public: InputEventFunction(const std::function<void(Window&)				>& func) : function(func) { } };

	struct KeyState
	{
		bool down;
		bool pressed;
		bool released;
		uint combo;
		double timePressed;
	};	

	namespace Input
	{				
		//BLAZE_API void SetEventFunction(const InputEventFunction::KeyDown			&);
		BLAZE_API void SetEventFunction(const InputEventFunction::KeyPressed		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::KeyReleased		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::MousePressed		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::MouseReleased		&);
		//BLAZE_API void SetEventFunction(const InputEventFunction::MouseDown			&);
		BLAZE_API void SetEventFunction(const InputEventFunction::MouseMotion		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::MouseMotion		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::MouseMotion		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::MouseScroll		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::MouseEnter		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::MouseLeave		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::WindowSizeChanged	&);
		BLAZE_API void SetEventFunction(const InputEventFunction::WindowResized		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::WindowMoved		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::WindowMinimized	&);
		BLAZE_API void SetEventFunction(const InputEventFunction::WindowMaximized	&);
		BLAZE_API void SetEventFunction(const InputEventFunction::WindowFocusGained	&);
		BLAZE_API void SetEventFunction(const InputEventFunction::WindowFocusLost	&);
		BLAZE_API void SetEventFunction(const InputEventFunction::WindowClosed		&);
		BLAZE_API void SetEventFunction(const InputEventFunction::TextInput			&);
	}
}