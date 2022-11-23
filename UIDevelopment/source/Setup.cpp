#include "ECS/Scene.h"
using namespace Blaze::ECS;

class Events : public EventHandler<Event::WindowResized>
{
public:
	void OnEvent(Event::WindowResized event)
	{
		Graphics::Core::SetViewport({ }, event.size);
		//s->SetProjectionMatrix(Math::OrthographicMatrix<float>(0, event.size.x, 0, event.size.y, -100, 100));
	}
};

class A : Component
{
public:
	int a;
	static constexpr const char* typeName = "A";
};

class B : Component
{
public:
	float b;
	static constexpr const char* typeName = "B";
};


CLIENT_API void Setup()
{	
	Events e;
	EventManager::AddEventHandler<Event::WindowResized>(e);

	ComponentTypeRegistry registry;
	registry.RegisterType<A>();
	registry.RegisterType<B>();

	Manager manager;
	manager.SetRegistry(registry);

	Scene scene;
	scene.SetManager(&manager);

	A* a;
	B* b;
	Entity* entity = scene.Create<A, B>(a, b);

	Window window;
	window.SetWindowed(true, false);
	window.ShowWindow(true);
	
	Graphics::Core::SetViewport({ }, window.GetSize());
	Graphics::Core::SetTarget(window);

	{
		FontResolution* res = Graphics::GetDefaultFont().CreateFontResolution(10, FontResolutionRenderType::HorizontalLCD);
		res->LoadCharacters(0, 128);
		res->CreateAtlas();
		Graphics::GetTextRenderer().SetFontResolution(res);
	}

	while (!Input::GetKeyState(Key::Escape).down)
	{
		Input::Update();

		//scene.Update();

		Graphics::Core::ClearTarget();

		//Graphics::Write((StringUTF8)r.FormatString(), Vec2f(0, window.GetSize().y - 16), 10, 0xffffffff);
		//scene.Render();

		Graphics::Core::SwapWindowBuffers();
	}
}