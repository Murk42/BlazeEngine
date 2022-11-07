#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application/UI System/UIElement.h"

namespace Blaze::UI
{
	class ListManager;
	struct ListProperties;

	struct ListElementData
	{
		UIElement* ptr = nullptr;
		int typeIndex = -1;
		float offset = 0;
		float topSpacing = 0;
		float bottomSpacing = 0;
	};

	struct ListProperties
	{
		float elementsOffset = 5;
		std::vector<ListElementData> elements;
	};

	class BLAZE_API List : public UIElement
	{	
		std::vector<ListElementData> elements;		
		float elementOffset;
		
		void InitializeElement(UIElement*, uint);
	public:
		List();

		void UpdateSize();		
		template<typename T>
		void AddElement(T* element, int location = -1, float offset = 0, float topSpacing = 0, float bottomSpacing = 0)
		{
			AddElement({ element, GetManager()->GetElementTypeRegistry().GetElementTypeIndex(T::typeName), offset, topSpacing, bottomSpacing }, location);
		}
		void AddElement(ListElementData data, int location = -1);
		void RemoveElement(uint location);
		void SetElements(const std::vector<ListElementData>& elements);
		
		void SetSize(Vec2f size) override;
		void SetActiveFlag(bool active) override;
		void SetSolidFlag(bool solid) override;
		void SetElementOffset(float offset) { elementOffset = offset; }

		const auto& GetElements() const { return elements; }

		void SetProperties(const ListProperties&);

		constexpr static const char* typeName = "List";
		using ManagerType = ListManager;

		friend class ListManager;
	};	

	class ListManager : public UIElementManager<List>
	{
	public:		
	};
}