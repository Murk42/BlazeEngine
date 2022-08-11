#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

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

void RenderCube()
{
	static Quatf cameraRot;
	Vec3f cameraPos = cameraRot * Vec3f(0, 0, -3);

	cameraRot *= Quatf(Vec3f(0, 1, 0), 0.001f);
	
	Graphics::Set3DViewMatrix(Math::RotationMatrix(cameraRot.Conjugated()) * Math::TranslationMatrix(-cameraPos));
	Graphics::DrawBoxOutline3D(Vec3f(-1, -1, -1), Vec3f(1, 1, 1), 0xFFFFFFFF, 3.0f);
}

CLIENT_API void Setup()
{
	Window window{ "Blaze engine application", Vec2i(960, 540) };
	window.SetWindowed(false, false);
	window.ShowWindow(true);

	Renderer::SetTarget(window);
	Renderer::SetViewport({}, window.GetSize());

	Resource::ResourceManager resourceManager;
	resourceManager.AddResource("Default font", &Graphics::GetDefaultFont());

	UI::UIScene scene;
	scene.SetResourceManager(&resourceManager);

	UI::Button* propertiesTextButton;
	UI::Text* propertiesText;
	UI::Text* smallText;


	scene.AddEventFunction("PropertiesTextSizeChanged", [&]() {		
		propertiesTextButton->SetSize(propertiesText->GetSize());
		});
	scene.AddEventFunction("PropertiesButtonPressed", [&]() {
		smallText->shown = !smallText->shown;
		});

	scene.Load("assets/scene");

	propertiesTextButton = scene.GetElement("button");
	propertiesText = (UI::Text*)scene.GetElement("propertiesText");
	smallText = (UI::Text*)scene.GetElement("smallText");


	while (!Input::GetKeyState(Key::Escape).pressed)
	{
		int FPS = CalculateFPS();

		Input::Update();

		scene.Update();

		Renderer::ClearTarget();

		scene.Render();

		RenderCube();

		Renderer::SwapWindowBuffers();
	}
};