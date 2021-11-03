#pragma once
#include "BlazeEngine/Core/EngineCore.h"

#include "BlazeEngine/Graphics/UI/States/ButtonState.h"
#include "BlazeEngine/Graphics/UI/Elements/Text.h"
#include "BlazeEngine/Graphics/UI/Elements/Panel.h"

namespace Blaze::UI
{
	struct ButtonPallete
	{
		ColorRGBA normalColor;
		ColorRGBA hoveredColor;
		ColorRGBA pressedColor;
		ColorRGBA outlineColor;
	};
	
	class TextRenderer;
	class PanelRenderer;

	class BLAZE_API Button
	{
		ButtonState state;
		Text text;
		const ButtonPallete* pallete;
	public:
		Panel panel;

		Button();

		void SetPallete(const ButtonPallete* pallete);

		void Add()
	};
}