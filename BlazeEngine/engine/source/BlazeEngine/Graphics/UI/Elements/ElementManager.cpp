#include "BlazeEngine/Graphics/UI/Elements/ElementManager.h"
#include "BlazeEngine/Graphics/UI/Elements/Element.h"

namespace Blaze::UI
{	

	void ElementManager::AddElement(Element& element)
	{
		element.manager = this;
		elements.emplace_back(&element);
	}

	void ElementManager::SetUIScale(float scale)
	{
		for (auto& element : elements)
			element->transform.scale = scale;
	}
}