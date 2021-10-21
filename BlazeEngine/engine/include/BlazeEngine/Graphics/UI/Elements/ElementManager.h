#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include <vector>

namespace Blaze::UI
{
	class Element;
	class Layer;

	class BLAZE_API ElementManager
	{		
		Layer* layer;
	public:
		std::vector<Element*> elements;		

		void AddElement(Element& element);
		
		void SetUIScale(float scale);

		friend class Layer;
		friend class Element;
	};
}