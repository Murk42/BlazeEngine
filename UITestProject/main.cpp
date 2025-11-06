#include "pch.h"
#include "MainScreen.h"

class Application;

class ApplicationLayer
{
public:
	virtual bool Update() { return false; }
	virtual void Render() {}
private:
};

class Application
{
public:
	~Application()
	{
		for (auto& layer : applicationLayers)
			delete layer;
	}
	template<typename T, typename ... Args>
	void AddLayer(Args&& ... args)
	{
		applicationLayers.AddFront(new T(std::forward<Args>(args)...));
	}

	void Run()
	{
		while (true)
		{
			bool exit = false;

			for (auto& layer : applicationLayers)
				exit |= !layer->Update();

			if (exit)
				break;

			for (auto& layer : applicationLayers)
				layer->Render();
		}
	}
private:
	List<ApplicationLayer*> applicationLayers;
};

template<auto MemberFunction>
class EventMemberFunctionTie
{
public:
	using Arguments = MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunction)>;
	using EventType = RemoveCVRef<typename Arguments::template TypeAtIndex<0>>;
	using ClassType = MemberFunctionPointerTypeClassType<decltype(MemberFunction)>;

	EventMemberFunctionTie(EventDispatcher<EventType>& dispatcher, ClassType& object)
		: dispatcher(dispatcher), object(object)
	{
		dispatcher.AddHandler<MemberFunction>(object);
	}
	~EventMemberFunctionTie()
	{
		dispatcher.RemoveHandler<MemberFunction>(object);
	}
private:
	EventDispatcher<EventType>& dispatcher;
	ClassType& object;
};
#define EVENT_MEMBER_FUNCTION(className, name, dispatcher) EventMemberFunctionTie<&className::name> name##Handler{ dispatcher, *this };

class MainApplicationLayer : public ApplicationLayer
{
public:
	ResourceManager resourceManager;

	Graphics::OpenGL::GraphicsContext_OpenGL graphicsContext{ { } };
	Graphics::OpenGL::RenderWindow_OpenGL window{ graphicsContext, WindowCreateOptions{.hidden = true, .title = "UI test" }, };

	Graphics::OpenGL::TexturedRectRenderer_OpenGL texturedRectRenderer{ graphicsContext };
	Graphics::OpenGL::PanelRenderer_OpenGL panelRenderer{ graphicsContext };
	Graphics::RendererRegistry rendererRegistry;

	UI::RenderSystem UIRenderSystem;
	MainScreen mainScreen{ &window, resourceManager};

	bool shouldExit = false;

	MainApplicationLayer()
	{
		rendererRegistry.RegisterRenderer(&texturedRectRenderer);
		rendererRegistry.RegisterRenderer(&panelRenderer);

		UIRenderSystem.SetRendererRegistry(rendererRegistry);
		UIRenderSystem.SetScreen(&mainScreen);

		Bitmap bm;
		bm.Load("assets/images/icon.png");
		window.SetIcon(bm);

		window.SetHiddenFlag(false);

		struct A
		{
			int a;
			float b;
			double c;
		};


	}
	~MainApplicationLayer()
	{
	}

	bool Update() override
	{
		Input::Update();

		if (Input::GetKeyFrameState(Input::Key::F4).down && Input::GetKeyFrameState(Input::Key::LALT).down)
		{
			shouldExit = true;
		}

		if (Input::GetKeyFrameState(Input::Key::F11).pressed)
		{
			if (window.GetWindowPresentMode() == WindowPresentMode::Fullscreen)
				window.SetWindowPresentMode(WindowPresentMode::Normal);
			else
				window.SetWindowPresentMode(WindowPresentMode::Fullscreen);
		}

		return !shouldExit;
	}
	void Render() override
	{
		window.ClearRenderBuffers();

		UIRenderSystem.Render(window.GetSize());

		window.Present();
	}

	void WindowClosed(const Window::WindowCloseEvent& event)
	{
		shouldExit = true;
	}
	EVENT_MEMBER_FUNCTION(MainApplicationLayer, WindowClosed, window.windowCloseEventDispatcher)
};

CLIENT_API void Setup()
{
	Debug::Logger::AddOutputFile("log.txt");

	Application app;
	app.AddLayer<MainApplicationLayer>();
	app.Run();
}