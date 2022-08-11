#pragma once
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/DataStructures/Common.h"
#include <vector>

namespace Blaze
{
	namespace UI
	{		
		class UIManager;
		
		class BLAZE_API UIBaseElementManager
		{
			UIManager* manager;
		
			virtual void AddElement(UIElement* el) = 0;
		public:
			virtual size_t GetElementCount() = 0;
			virtual UIElement* GetElementBase(size_t index) = 0;

			virtual void Setup() { }
			virtual void Cleanup() { }

			virtual void Render(size_t start, size_t end) { }
			virtual void Update(size_t start, size_t end);

			UIManager* GetManager() const { return manager; }

			template<typename T>
			friend class UIElementManager;
			friend class UIManager;
		};

		template<typename T>
		class UIElementManager : public UIBaseElementManager
		{
			std::vector<T*> elements;

			void AddElement(UIElement* el) override
			{
				elements.emplace_back((T*)el);
			}
		public:
			size_t GetElementCount() override
			{
				return elements.size();
			}			
			UIElement* GetElementBase(size_t index) override
			{
				return elements[index];
			}
			T* GetElement(uint index)
			{
				return elements[index];
			}

			using ElementType = T;
		};
	}
}