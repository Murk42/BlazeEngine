#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Color.h"

#include "BlazeEngine/Graphics/UI/Elements/Element.h"

namespace Blaze::UI
{
	class PanelRenderer;

	class BLAZE_API Panel : public Element
	{
		PanelRenderer* renderer;
	public:
		ColorRGBA color;
		ColorRGBA outlineColor;
		float radius;
		float thickness;

		Panel();

		friend class PanelRenderer;
	};
}