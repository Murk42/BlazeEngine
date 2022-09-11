#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Application/UI System/UIElementTypeRegistry.h"
#include "BlazeEngine/Application/UI System/UIElementManager.h"
#include "BlazeEngine/Application/UI System/UIAlignment.h"

namespace Blaze
{
	namespace UI
	{		
		class BLAZE_API UIManager
		{		
			UIElementTypeRegistry registry;			
			size_t typeCount;
			UIBaseElementManager** managers;
			std::vector<uint*> layers;
			
			bool focusTaken = false;
			bool blocked = false;			

			void CleanElement(UIElement*);

			uint32 updateState;
		public:
			UIManager();	
			~UIManager();

			void SetElementTypeRegistry(const UIElementTypeRegistry& typeRegistry);			
			uint CreateLayer();

			template<typename T>
			Result SetElementManager(UIBaseElementManager* manager)
			{				
				return SetElementManager(manager, registry.GetElementTypeIndex<T>());
			}
			Result SetElementManager(UIBaseElementManager* manager, uint typeIndex);			

			template<typename T>
			Result AddElement(T* ptr, uint layer)
			{
				return AddElement(ptr, registry.GetElementTypeIndex<T>(), layer);
			}
			Result AddElement(UIElement* ptr, uint typeIndex, uint layer);			

			Result SetupElementManagers();
			Result CleanupElementManagers();

			Result Render();
			Result Update();
			void DrawDebugLines() const;

			bool TakeFocus();
			void ReturnFocus();

			bool IsFocusTaken() const { return focusTaken; }
			bool IsMouseBlocked() const { return blocked; }			
			size_t GetElementTypeCount() const { return typeCount; }
			const UIElementTypeRegistry& GetElementTypeRegistry() const { return registry; }			
		};
	}
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