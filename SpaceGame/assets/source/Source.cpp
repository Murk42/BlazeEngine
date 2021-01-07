#include "BlazeEngine/BlazeEngineMain.h"
using namespace Blaze;

#include "Scenes/GameScene.h"

static void WindowCloseFunction(Window*); 
static void WindowResizeFunction(int, int, Window*);

class App : public Application<App>
{
public:
	Console::List logList = Console::List({ 0, 0 }, { 100, 20 });
	Window win;

	void Startup() override
	{			
		Input::AddEventFunction(WindowClose, WindowCloseFunction);
		Input::AddEventFunction(WindowResized, WindowResizeFunction);
		win.Maximize();
		WindowResizeFunction(win.GetSize().x, win.GetSize().y, &win);
		Renderer::SetTarget(win); 
		    
		SceneManager::LoadScene<GameScene>(); 
		SceneManager::RunScene<GameScene>();

		win.ShowWindow(true);
	}

	void Cleanup() override
	{

	}

	Timer frameTimer;
	void Frame() override
	{
		Input::Update();		

		SceneManager::Update(frameTimer.Reset());

		Log log;
		while (Logger::GetNextLog(log))		
			logList.Add(log.source + ": " + log.message);		

		if (log.message.Size() != 0)
		{
			logList.Clear();   
			logList.Refresh();
		}
	}
};
         
static void WindowCloseFunction(Window*)
{
	BaseApplication::Stop();
}
static void WindowResizeFunction(int x, int y, Window*)  
{
	Renderer::SetViewport(Vec2i(0, 0), Vec2i(x, y));
}