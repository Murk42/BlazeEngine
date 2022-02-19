#include "BlazeEngine/Event/EventManager.h"

namespace Blaze
{
	EventDispatcher<Event::KeyPressed		 >        keyPressedDispatcher;
	EventDispatcher<Event::KeyReleased		 >       keyReleasedDispatcher;
	EventDispatcher<Event::MousePressed	     >      mousePressedDispatcher;
	EventDispatcher<Event::MouseReleased	 >     mouseReleasedDispatcher;
	EventDispatcher<Event::MouseMotion	     >       mouseMotionDispatcher;
	EventDispatcher<Event::MouseScroll	     >       mouseScrollDispatcher;
	EventDispatcher<Event::MouseEnter	     >        mouseEnterDispatcher;
	EventDispatcher<Event::MouseLeave	     >        mouseLeaveDispatcher;
	EventDispatcher<Event::WindowResized	 >     windowResizedDispatcher;
	EventDispatcher<Event::WindowMoved		 >       windowMovedDispatcher;
	EventDispatcher<Event::WindowMinimized	 >   windowMinimizedDispatcher;
	EventDispatcher<Event::WindowMaximized	 >   windowMaximizedDispatcher;
	EventDispatcher<Event::WindowFocusGained > windowFocusGainedDispatcher;
	EventDispatcher<Event::WindowFocusLost	 >   windowFocusLostDispatcher;
	EventDispatcher<Event::WindowClosed	     >      windowClosedDispatcher;
	EventDispatcher<Event::TextInput		 >         textInputDispatcher;
	EventDispatcher<Event::ViewportChanged   >   viewportChangedDispatcher;

	namespace EventManager
	{

		template<> void AddHandler<Event::KeyPressed		 >(EventHandler<Event::KeyPressed		>& handler) {        keyPressedDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::KeyReleased		 >(EventHandler<Event::KeyReleased		>& handler) {       keyReleasedDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::MousePressed	     >(EventHandler<Event::MousePressed		>& handler) {      mousePressedDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::MouseReleased	     >(EventHandler<Event::MouseReleased	>& handler) {     mouseReleasedDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::MouseMotion	     >(EventHandler<Event::MouseMotion		>& handler) {       mouseMotionDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::MouseScroll	     >(EventHandler<Event::MouseScroll		>& handler) {       mouseScrollDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::MouseEnter	     >(EventHandler<Event::MouseEnter		>& handler) {        mouseEnterDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::MouseLeave	     >(EventHandler<Event::MouseLeave		>& handler) {        mouseLeaveDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::ViewportChanged    >(EventHandler<Event::ViewportChanged	>& handler) {   viewportChangedDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::WindowResized	     >(EventHandler<Event::WindowResized	>& handler) {     windowResizedDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::WindowMoved		 >(EventHandler<Event::WindowMoved		>& handler) {       windowMovedDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::WindowMinimized	 >(EventHandler<Event::WindowMinimized	>& handler) {   windowMinimizedDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::WindowMaximized	 >(EventHandler<Event::WindowMaximized	>& handler) {   windowMaximizedDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::WindowFocusGained  >(EventHandler<Event::WindowFocusGained>& handler) { windowFocusGainedDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::WindowFocusLost	 >(EventHandler<Event::WindowFocusLost	>& handler) {   windowFocusLostDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::WindowClosed	     >(EventHandler<Event::WindowClosed		>& handler) {      windowClosedDispatcher.AddHandler(handler); }
		template<> void AddHandler<Event::TextInput		     >(EventHandler<Event::TextInput		>& handler) {         textInputDispatcher.AddHandler(handler); }

	}
}