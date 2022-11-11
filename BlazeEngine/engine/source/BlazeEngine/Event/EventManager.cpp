#include "BlazeEngine/Event/EventManager.h"
#include "source/BlazeEngine/Internal/EngineData.h"

#define PerEvent(ev, name) \
	namespace EventManager { \
		template<> void AddEventHandler(Blaze::EventHandler<ev>& handler) { engineData->name.AddHandler(handler); } \
		template<> void RemoveEventHandler(Blaze::EventHandler<ev>& handler) { engineData->name.RemoveHandler(handler); } \
	}

namespace Blaze
{	
	PerEvent(Event::KeyPressed			, keyPressedDispatcher			)
	PerEvent(Event::KeyReleased			, keyReleasedDispatcher			)
	PerEvent(Event::MousePressed		, mousePressedDispatcher		)
	PerEvent(Event::MouseReleased		, mouseReleasedDispatcher		)
	PerEvent(Event::MouseMotion			, mouseMotionDispatcher			)
	PerEvent(Event::MouseScroll			, mouseScrollDispatcher			)
	PerEvent(Event::MouseEnter			, mouseEnterDispatcher			)
	PerEvent(Event::MouseLeave			, mouseLeaveDispatcher			)
	PerEvent(Event::WindowResized		, windowResizedDispatcher		)
	PerEvent(Event::WindowMoved			, windowMovedDispatcher			)
	PerEvent(Event::WindowMinimized		, windowMinimizedDispatcher		)
	PerEvent(Event::WindowMaximized		, windowMaximizedDispatcher		)
	PerEvent(Event::WindowFocusGained	, windowFocusGainedDispatcher	)
	PerEvent(Event::WindowFocusLost		, windowFocusLostDispatcher		)
	PerEvent(Event::WindowClosed		, windowClosedDispatcher		)
	PerEvent(Event::TextInput			, textInputDispatcher			)
	PerEvent(Event::ViewportChanged		, viewportChangedDispatcher		)	
}