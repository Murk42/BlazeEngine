#pragma once
#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Application/UI System/UIElementTypeRegistry.h"
#include "BlazeEngine/Application/UI System/UIElementManager.h"
#include "BlazeEngine/Application/UI System/UIEvent.h"

#include <queue>

namespace Blaze::UI
{	
	class BLAZE_API UIManager
	{
	public:
		struct Layer
		{
			String name;
			std::vector<UIElement*> elements;
		};
	private:
		UIElementTypeRegistry registry;
		size_t typeCount;
		UIBaseElementManager** managers;
		
		std::unordered_map<String, std::list<Layer>::iterator> layerNames;
		std::list<Layer> layers;
		
		UIElement* focusedElement;
		UIElement* blockingElement;

		std::queue<const UIEvent*> eventQueue;

		void ResolveClipRect(UIElement* element);
		void ResolveAlignedRect(UIElement* element);
		void CleanElement(UIElement*);

		uint32 updateState;

		Recti viewport;
		Mat4f vp2d;
		
		Result SetupElementManagers();
		Result CleanupElementManagers();

		UIEvent viewportChanged;
	public:
		UIManager();
		~UIManager();

		Result SetElementTypeRegistry(const UIElementTypeRegistry& typeRegistry);

		template<typename T>
		Result AddElement(T* ptr, StringView layer)
		{
			return AddElement(ptr, registry.GetElementTypeIndex<T>(), layer);
		}
		Result AddElement(UIElement* ptr, uint typeIndex, StringView layer);
		Result RemoveElement(UIElement* ptr);

		void AddEventToQueue(const UIEvent& event);

		Result CreateLayer(StringView name);
		Result CreateLayerAfter(StringView name, StringView relative);
		Result CreateLayerBefore(StringView name, StringView relative);
		bool HasLayer(StringView layer);

		Result Render();
		Result Update();
		void DrawDebugLines() const;

		bool TakeFocus(UIElement* focusedElement);
		void ReturnFocus();

		void SetViewport(Vec2i pos, Vec2i size);

		void AddViewportChangedFunction(UIEventFunction function);

		template<typename T>
		typename T::ManagerType& GetElementManager()
		{
			return (typename T::ManagerType&)GetElementManager(registry.GetElementTypeIndex<T>());
		}
		UIBaseElementManager& GetElementManager(uint typeIndex);

		bool IsFocusTaken() const { return focusedElement != nullptr; }
		UIElement* GetFocusedElement() const { return focusedElement; }

		bool IsMouseBlocked() const { return blockingElement != nullptr; }
		UIElement* GetBlockingElement() const { return blockingElement; }

		size_t GetElementTypeCount() const { return typeCount; }
		const UIElementTypeRegistry& GetElementTypeRegistry() const { return registry; }

		Recti GetViewport() const { return viewport; }
		const Mat4f& GetProjectionMatrix() const { return vp2d; }

		auto& GetLayers() { return layers; }
		auto& GetLayers() const { return layers; }

		friend class UIElement;
	};
}

/*
					
--------------ElementTypeRegistry--------------------
Types count: 3

 ID |  Name  |
----+--------+
  0 | Text   |
  1 | Panel  |
  2 | Image  |
  3 | Button |
-----------------------------------------------------

-------------------BaseElement-----------------------
-----------------------------------------------------

------------------ElementManager---------------------
Manager: manager

Elements(bucket list): img1, img2, img3, img4
-----------------------------------------------------

----------------------Layer--------------------------
Manager: manager

 ID | Count |
----+-------+
  0 |    1  |
  1 |    1  |
  2 |    2  |
  3 |    0  |
-----------------------------------------------------

---------------------Manager-------------------------
ElementTypeRegistry: 

Managers:
 ID |    Manager    |
----+---------------+
  0 |  TextManager  |
  1 | PanelManager  |
  2 | ImageManager  |
  3 |      x        |

Layers(bucket list): l1, l2, l3
-----------------------------------------------------

*/