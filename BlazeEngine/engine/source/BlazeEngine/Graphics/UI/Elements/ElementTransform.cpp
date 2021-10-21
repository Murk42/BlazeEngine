#include "BlazeEngine/Graphics/UI/Elements/ElementTransform.h"

namespace Blaze::UI
{
	ElementTransform::ElementTransform()
		: depth(0.0f), scale(1.0f), align(Align::Center), parentAlign(Align::Center)
	{
	}
}