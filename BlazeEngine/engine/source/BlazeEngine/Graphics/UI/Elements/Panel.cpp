#include "BlazeEngine/Graphics/UI/Elements/Panel.h"
#include "BlazeEngine/Graphics/UI/Elements/PanelRenderer.h"

namespace Blaze::UI
{
	Panel::Panel()
		: radius(8.0f), thickness(2.0f), color(255, 255, 255, 255), outlineColor(255, 0, 0, 255)
	{
	}
}