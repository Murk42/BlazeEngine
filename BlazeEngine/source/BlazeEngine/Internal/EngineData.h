#pragma once
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/Utilities/Timing.h"
#include "SDL2/SDL_scancode.h"

namespace Blaze
{
	class Window;
	namespace Debug
	{
		class LoggerListener;
	}
}

struct EngineData
{	
	Blaze::List<Blaze::Debug::LoggerListener*> loggerListeners;
	
	Blaze::Map<Blaze::Key, SDL_Scancode> scancodemap;
	Blaze::Map<SDL_Scancode, Blaze::Key> keymap;

	Blaze::Map<std::thread::id, Blaze::uint> threadIDMap;

	Blaze::TimingResult blazeInitTimings;

	Blaze::Result OpenGLResult;

	bool finishedInit = false;
};
extern EngineData* engineData;

bool FlushOpenGLResult();

#define CHECK_OPENGL_RESULT() if(::engineData->OpenGLResult) return std::move(::engineData->OpenGLResult);