#include "BlazeEngine/BlazeEngineMain.h"
using namespace Blaze;

class App : public Application<App>
{
public:
	Console::List logList = Console::List({ 0, 0 }, { 100, 20 });
	Window win;

	void Startup() override
	{
		win.ShowWindow(true);
		Renderer::SetTarget(win);

		SetupEvents();
	}

	void Cleanup() override
	{

	}

	void Frame() override
	{
		Input::Update();		

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

#include "Events.h"

static void SetupEvents()
{
	WindowResizeFunc(App::Instance()->win.GetSize().x, App::Instance()->win.GetSize().y, &App::Instance()->win);
	Input::AddEventFunction(WindowResized, WindowResizeFunc);
	Input::AddEventFunction(WindowClose, WindowCloseFunc);
}