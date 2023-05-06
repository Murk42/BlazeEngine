#pragma once
#include <BlazeEngine/Event/EventDispatcher.h>
#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/Core/Startup.h"
#include "SDL/SDL_scancode.h"
#include <thread>
#include <map>
#include <memory>

namespace Blaze
{
	class Window;
	class LoggerListener;
}

struct EngineData
{
	Blaze::EventDispatcher<Blaze::Event::KeyPressed			> keyPressedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::KeyReleased		> keyReleasedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MousePressed		> mousePressedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MouseReleased		> mouseReleasedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MouseMotion		> mouseMotionDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MouseScroll		> mouseScrollDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MouseEnter			> mouseEnterDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MouseLeave			> mouseLeaveDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowResized		> windowResizedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowMoved		> windowMovedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowMinimized	> windowMinimizedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowMaximized	> windowMaximizedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowFocusGained	> windowFocusGainedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowFocusLost	> windowFocusLostDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowClosed		> windowClosedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::TextInput			> textInputDispatcher;
	Blaze::EventDispatcher<Blaze::Event::ViewportChanged	> viewportChangedDispatcher;	

	std::list<Blaze::LoggerListener*> loggerListeners;

	std::map<Blaze::Key, SDL_Scancode> scancodemap;
	std::map<SDL_Scancode, Blaze::Key> keymap;

	std::vector<Blaze::Window*> allWindows;

	std::unordered_map<std::thread::id, Blaze::uint> threadIDMap;

	Blaze::Startup::BlazeInitInfo initInfo;

	Blaze::Result OpenGLResult;

	bool finishedInit = false;
};
extern EngineData* engineData;

#define FLUSH_OPENGL_RESULT() (ADD_STACK_FRAME((Blaze::Result(std::move(engineData->OpenGLResult)))))
#define CHECK_OPENGL_RESULT() CHECK_RESULT(engineData->OpenGLResult)