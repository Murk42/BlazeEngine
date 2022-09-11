#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

namespace Blaze::UI
{
	class ListManager;

	class List : public UIElement
	{
		std::vector<std::pair<UIElement*, uint>> elements;
		float customSize = -1;
				
	public:
		void UpdateSize();		
		template<typename T>
		void AddElement(T* element)
		{
			AddElement(element, GetManager()->GetElementTypeRegistry().GetElementTypeIndex(T::typeName));
		}
		void AddElement(UIElement* element, uint typeIndex);

		void SetRect(Rectf rect) override;		
		void SetSize(Vec2f size) override;
		void SetActiveFlag(bool active) override;

		constexpr static const char* typeName = "List";
		using ManagerType = ListManager;
	};

	class ListManager : public UIElementManager<List>
	{
	public:
		static UIElementParsingData GetElementParsingData();
	};
}