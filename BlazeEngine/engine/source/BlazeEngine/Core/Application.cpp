#include "BlazeEngine/Core/Application.h"

#include "source/BlazeEngine/Internal/Engine.h"
#include "source/BlazeEngine/Internal/Libraries.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Core/Scene.h"

#ifdef __WINDOWS__
#include <Windows.h>
#endif

#undef GetMessage


namespace Blaze
{
	SceneInfo startupSceneInfo;

	void StartBlazeApplication(SceneInfo sceneInfo)
	{						
		startupSceneInfo = sceneInfo;
	}

	enum class StateFlags
	{
		Initialized = 0x1,
		Running = 0x2,
	};

	enum class InitFlags
	{
		SDL			= 0x1,
		GLEW		= 0x2,
		DevIL		= 0x4,
		PDC			= 0x8,
		FreeType	= 0x10,
		Blaze		= 0x20,
	};

	namespace Logger
	{
		void OpenGLCallbackFunc(unsigned, unsigned, int, unsigned, unsigned, const char*);
	}	

	namespace Console
	{
		void ConsoleInputThreadFunction();
	}	

	namespace Input
	{
		void Update();
	}

	namespace Application
	{
		void Stop()
		{
			engine->ProcessInfo.running = false;
		}

		void SetTargetFramerate(uint fps)
		{
			engine->FrameInfo.targetDeltaTime = 1.0f / double(fps);
		}
		void SetTargetDeltaTime(double deltaTime)
		{
			engine->FrameInfo.targetDeltaTime = deltaTime;
		}		

		uint GetTargetDeltaTime()
		{
			return engine->FrameInfo.targetDeltaTime;
		}
		uint GetFPS()
		{
			return engine->FrameInfo.FPS;
		}
		double GetDeltaTime()
		{
			return engine->FrameInfo.deltaTime;
		}
	}

	void NewLog(const Log& log)
	{
		Console::Write("(file: \"" + log.GetFilePath() + "\")\n[" + String::Convert(log.GetThreadID()) + "] (" + log.GetFunctionName() + ", " + String::Convert(log.GetLine()) + ") " + log.GetSource() + ":\n" + log.GetMessage() + "\n");
	}	
	
	void InitializeBlaze()
	{
		SDL_GetGlobalMouseState(&engine->Input.mousePos.x, &engine->Input.mousePos.y);

		engine->ProcessInfo.threadIDMap[std::this_thread::get_id()] = 0;

		//Input
		{
			engine->Console.Input.thread = std::move(std::thread(Console::ConsoleInputThreadFunction));

			for (auto& keyState : engine->Input.keyStates)
			{
				keyState.timePressed = 0;
				keyState.combo = 0;
				keyState.down = false;
				keyState.pressed = false;
				keyState.released = false;
			}
			for (auto& keyState : engine->Input.mouseKeyStates)
			{
				keyState.timePressed = 0;
				keyState.combo = 0;
				keyState.down = false;
				keyState.pressed = false;
				keyState.released = false;
			}


		}

		if (startupSceneInfo.constructor != nullptr)
		{
			engine->ProcessInfo.running = true;
			engine->Scene.startupSceneInfo = startupSceneInfo;

			engine->Scene.currentSceneInfo = engine->Scene.startupSceneInfo;
			engine->Scene.currentScene = malloc(engine->Scene.currentSceneInfo.size);
			engine->Scene.currentSceneInfo.constructor(engine->Scene.currentScene);
		}
	}
	void TerminateBlaze()
	{			
		engine->Console.Input.m.lock();
		engine->Console.Input.exit = true;
		engine->Console.Input.m.unlock();
		engine->Console.Input.thread.detach();			

		if (startupSceneInfo.constructor != nullptr)
		{
			engine->Scene.currentSceneInfo.destructor(engine->Scene.currentScene);
			free(engine->Scene.currentScene);
		}
	}

	void Run()
	{
		if (engine->Scene.currentScene != nullptr)
			((SceneBase*)engine->Scene.currentScene)->Startup();						
		
		while (engine->ProcessInfo.running)
		{
			std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> frameDuration = now - engine->FrameInfo.lastFrame;
			engine->FrameInfo.deltaTime = frameDuration.count();
			engine->FrameInfo.lastFrame = now;
						
			std::chrono::duration<double> FPSDuration = now - engine->FrameInfo.FPStime_point;
			if (FPSDuration.count() > 1.0f)
			{
				engine->FrameInfo.FPS = engine->FrameInfo.FPScounter;
				engine->FrameInfo.FPScounter = 0;
				engine->FrameInfo.FPStime_point = now;
			}
			++engine->FrameInfo.FPScounter;


			Input::Update();

			((SceneBase*)engine->Scene.currentScene)->Update();

			std::this_thread::sleep_until(now + std::chrono::duration<double>(std::chrono::microseconds(uint(engine->FrameInfo.targetDeltaTime * 1000000))));			
		}
		
		if (engine->Scene.currentScene != nullptr)
			((SceneBase*)engine->Scene.currentScene)->Cleanup();		
	}
}

int main()
{	
	try
	{
		Blaze::Buffer engineBuffer;

		engineBuffer.Allocate(sizeof(Blaze::Engine));
		new (engineBuffer.Ptr())Blaze::Engine();
		
		Blaze::engine = (Blaze::Engine*)engineBuffer.Ptr();
		
		Blaze::Run();		

		((Blaze::Engine*)engineBuffer.Ptr())->~Engine();
		engineBuffer.Free();
	}
	catch (const Blaze::String& message)
	{
#ifdef __WINDOWS__	
		HWND hwnd = GetActiveWindow();
		MessageBox(NULL, (LPCSTR)message.Ptr(), NULL, MB_ICONERROR | MB_OK);
#endif
		return 0;
	}
	catch (const char* message)
	{
#ifdef __WINDOWS__	
		HWND hwnd = GetActiveWindow();
		MessageBox(NULL, (LPCSTR)message, NULL, MB_ICONERROR | MB_OK);
#endif
		return 0;
	}
	catch (const std::exception& exception)
	{
#ifdef __WINDOWS__	
		HWND hwnd = GetActiveWindow();
		MessageBox(NULL, (LPCSTR)exception.what(), NULL, MB_ICONERROR | MB_OK);
#endif
		return 0;
	}
	catch (...)
	{
#ifdef __WINDOWS__	
		HWND hwnd = GetActiveWindow();
		MessageBox(NULL, "An unknown exception was thrown", NULL, MB_ICONERROR | MB_OK);
#endif
		return 0;
	}	

	return 0;
}