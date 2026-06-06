#include "pch.h"
#include "Layers/MainLayer.h"

MainLayer::MainLayer()
{	 
	fontManager.AddFontFace("default", resourceManager.LoadResource<UI::FontFace>(UI::FontFace::LoadDefault()));
}

MainLayer::~MainLayer()
{
}

void MainLayer::SetupGraphics(const GraphicsAppRuntimeThreadData& data)
{
	UISystem.InitializeGraphics(data.graphicsContext, data.rendererRegistry);
	UISystem.SetWindow(&data.window);

	if (auto textRenderer = UISystem.GetGraphicsSubSystem().GetRendererRegistry().GetRenderer<Graphics::AntialiasedTextRenderer>())
		fontManager.CreateFontAtlas("default", { 48, 32, 27, 18, 12, 9, 6 }, *textRenderer);

	screen.windowTitleTextInput.SetValueEnteredCallback([&](u8StringView text)
		{
			data.window.SetTitle(text);
		});
} 

void MainLayer::Update() 
{
	screen.Update();
}

Input::EventProcessedState MainLayer::OnEvent(const Input::GenericInputEvent& event, bool processed)
{
	if (event.TryProcess([&](const Input::KeyDownEvent& event)
		{
			if (event.key == Input::Key::F11)
			{
				if (event.window.GetWindowPresentMode() == WindowPresentMode::Fullscreen)
					event.window.SetWindowPresentMode(WindowPresentMode::Normal);
				else
					event.window.SetWindowPresentMode(WindowPresentMode::Fullscreen);
				return true;
			}
			return false;
		}))
		return Input::EventProcessedState::Consumed;

	if (auto result = UISystem.ProcessInputEvent(event, processed))
		return result;

	return {};
}

void MainLayer::Render(const Graphics::RenderContext& renderContext)
{
	UISystem.Render(renderContext);
}
