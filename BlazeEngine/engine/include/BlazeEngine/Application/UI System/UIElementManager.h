#pragma once
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/BinaryInputStream.h"
#include "BlazeEngine/DataStructures/BinaryOutputStream.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/DataStructures/Color.h"
#include <vector>

namespace Blaze
{
	namespace UI
	{		
		class UIElement;
		class UIManager;
		class UIScene;

		struct UIDebugRenderData
		{
			struct ElementData
			{
				Rectf rect;
				float thickness;
				bool overrideLayerColor;
				ColorRGBAf color;
			};

			std::vector<ElementData> elements;
		};
		
		class BLAZE_API UIBaseElementManager
		{
			UIManager* manager = nullptr;
		public:			
			virtual void Setup() { }
			virtual void Cleanup() { }

			virtual void Render(UIElement*) { }
			virtual void Update(UIElement*) { }

			virtual UIDebugRenderData GetDebugRenderData(UIElement*);

			virtual void Serialize(UIElement*, BinaryOutputStream&);
			virtual void Deserialize(UIElement*, BinaryInputStream&);

			UIManager* GetManager() const { return manager; }

			template<typename T>
			friend class UIElementManager;
			friend class UIManager;
		};

		template<typename T>
		class UIElementManager : public UIBaseElementManager
		{
		public:			

			using ElementType = T;
		};
	}
}