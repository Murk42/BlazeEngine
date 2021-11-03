#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/UI/Elements/ElementTransform.h"

#include <functional>

namespace Blaze::UI
{	
	class BLAZE_API Element
	{
		std::vector<Element*> children;
		Element* parent;		
	public:
		ElementTransform transform;
		std::function<void()> TransformUpdate;

		Element();		

		void SetParent(Element& parent);
		void UpdateTransform();		
		
	};
}