#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "UI/List.h"
#include "UI/Slider.h"
#include "UI/Dropdown.h"

int CalculateFPS()
{
	static int fps_count;
	static int fps;
	static TimePoint last_time_point = TimePoint::GetWorldTime();

	TimePoint now = TimePoint::GetWorldTime();
	if (now - last_time_point > 1.0f)
	{
		fps = fps_count;
		last_time_point = now;
		fps_count = 0;
	}
	fps_count++;

	return fps;
}

void RenderFrames(int fps)
{
	Vec2i viewportSize = Renderer::GetViewportSize();

	Graphics::Write("FPS: " + StringUTF8::Convert(fps), Vec2f(5, 5), 32, 0x00ff00ff);
}

void RenderCube()
{
	static Quatf cameraRot;
	Vec3f cameraPos = cameraRot * Vec3f(0, 0, -3);

	cameraRot *= Quatf(Vec3f(0, 1, 0), 0.001f);
	cameraRot.Normalise();	

	Graphics::Set3DViewMatrix(cameraPos, cameraRot);
	Graphics::DrawBoxOutline3D(Vec3f(-1, -1, -1), Vec3f(1, 1, 1), 0xffffffff, 1.0f);
}

CLIENT_API void Setup()
{
	Window window{ "Blaze engine application", Vec2i(640, 360) };
	window.SetWindowed(false, false);
	window.ShowWindow(true);

	Renderer::SetTarget(window);
	Renderer::SetViewport({}, window.GetSize());

	Graphics::Core::Texture2D tex;
	tex.Load("assets/default/default.png");
	tex.SetSettings(Graphics::Core::Texture2DSettings());	

	Resource::ResourceManager resourceManager;
	resourceManager.AddResource("Default font", &Graphics::GetDefaultFont());

	UI::UIScene scene;

	UI::UIElementTypeRegistry registry;
	registry.RegisterCoreTypes();
	registry.RegisterType<UI::List>();
	registry.RegisterType<UI::Slider>();
	registry.RegisterType<UI::Dropdown>();

	scene.SetElementTypeRegistry(registry);	

	scene.AddElementParsingData<UI::List>();
	scene.AddElementParsingData<UI::Slider>();
	scene.AddElementParsingData<UI::Dropdown>();

	scene.SetResourceManager(&resourceManager);
	scene.AddCoreElementsParsingData();

	scene.AddEventFunction("valueChanged", [&]() {
		UI::UIElement* panel = scene.GetElement("propertiesPanel").first;
		UI::Slider* slider = (UI::Slider*)scene.GetElement("slider").first;
		panel->SetPos({ 200 - slider->GetValue() * 150, -10 });
		});

	scene.Load("assets/scene");

	Renderer::EnableBlending(true);

	while (!Input::GetKeyState(Key::Escape).pressed)
	{
		int FPS = CalculateFPS();

		Input::Update();

		scene.Update();

		Renderer::ClearTarget();

		//Graphics::DrawTexture(Vec2f(100, 100), Vec2f(300, 300), tex);
		Graphics::DrawLine3D(Vec3f(.5, .5, 10), Vec3f(-.5, -.5, -.5), 0xff0000ff, 2);
		RenderFrames(FPS);
		RenderCube();

		scene.Render();

		Renderer::SwapWindowBuffers();
	}
}