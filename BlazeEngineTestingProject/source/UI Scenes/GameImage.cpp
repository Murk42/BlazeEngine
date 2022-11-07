#include "Globals.h"

namespace GameImage
{
	UI::UIElement* gameImage;
	UI::Panel* gameImagePanel;

	void Setup()
	{		
		const char* layer = "GameImage";
		
		uiManager.CreateLayer(layer);

		gameImage = mainScene->CreateElement<UI::UIElement>("gameImage", layer);

		gameImage->UIElement::SetProperties({
			.pos = Vec2f(0, -50), .size = Vec2f(300, 300), .localAlign = Align::Top, .anchorAlign = Align::Top
			});
		gameImage->sizeChanged += [&]()
		{
			gameUIManager.SetViewport((Vec2i)gameImage->GetViewportPos(), (Vec2i)gameImage->GetSize());
		};
		
	}
}