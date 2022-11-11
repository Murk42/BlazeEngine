#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/Application/UI System/UIEvent.h"
#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/DataStructures/Array.h"
#include "BlazeEngine/DataStructures/String.h"
#include <bitset>

namespace Blaze
{
	namespace UI
	{
		class UIBaseElementManager;
		class UIManager;
		class UIScene;

		template<typename T>
		struct UIElementProperty
		{
			T value;
			bool edited;
		
			UIElementProperty()
				: edited(false), value()
			{
				if constexpr (std::is_pointer_v<T>)
					value = nullptr;
				if constexpr (std::is_floating_point_v<T> || std::is_integral_v<T>)
					value = 0;
			}
			UIElementProperty(const T& v)
			{
				value = v;
				edited = true;
			}
			UIElementProperty(T&& v) noexcept
			{
				value = std::move(v);
				edited = true;
			}
			UIElementProperty(const UIElementProperty& p)
			{
				if (p.edited)
				{
					value = p.value;
					edited = true;
				}
				else
					edited = false;
			}
			UIElementProperty(UIElementProperty&& p) noexcept
			{
				value = std::move(p.value);
				edited = p.edited;
			}

			UIElementProperty& operator=(const T& v)
			{
				value = v;
				edited = true;
				return *this;
			}
			UIElementProperty& operator=(T&& v)
			{
				value = std::move(v);
				edited = true;
				return *this;
			}
			UIElementProperty& operator=(const UIElementProperty& p)
			{
				if (p.edited)
				{
					value = p.value;
					edited = true;
				}
				return *this;
			}
			UIElementProperty& operator=(UIElementProperty&& p) noexcept
			{
				if (p.edited)
				{
					value = std::move(p.value);
					edited = true;
				}
				return *this;
			}

			operator T&()
			{
				return value;
			}
			operator const T&() const
			{
				return value;
			}
		};

		struct UIElementProperties
		{
			UIElementProperty<String> name;
			UIElementProperty<Vec2f> pos;
			UIElementProperty<Vec2f> size;
			UIElementProperty<UIElement*> anchor;
			UIElementProperty<Align> localAlign;
			UIElementProperty<Align> anchorAlign;
			UIElementProperty<bool> active;
			UIElementProperty<bool> solid;
			UIElementProperty<float> depth;
			UIElementProperty<UIElement*> clipElement;
		};

		class BLAZE_API UIElement
		{
			uint typeIndex;
			UIManager* manager;
			uint64 updateState;

			UIScene* scene;
			String name;

			StringView layer;
			
			Vec2f viewportPos;
			Vec2f pos;
			Vec2f size;
			
			float depth;
			std::bitset<16> flags;

			Align localAlignment;
			Align anchorAlignment;
			UIElement* anchor;

			UIElement* clipElement;
			Rectf clipRect;

			Array<UIElement*> tiedElements;
			UIElement* tiedParent;
		protected:
			virtual void AttachedToManager() { }
			virtual void DetachedFromManager() { }

			//template<typename T>
			//Result TieElement(T& element)
			//{
			//	if (manager == nullptr)
			//		return BLAZE_ERROR_LOG("BlazeEngine", "The element doesnt belong to a manager");
			//
			//	TieElement(&element, manager->GetElementTypeRegistry().GetElementTypeIndex<T>());
			//}						
			Result TieElement(UIElement* element);
			Result UntieElement(UIElement* element);
			//void UntieElement(UIElement* element);
		public:
			UIElement();
			~UIElement();

			UIEvent sizeChanged;

			virtual Result ChangeLayer(StringView newLayer);

			virtual void SetName(String name);
			virtual void SetPos(Vec2f pos);
			virtual void SetSize(Vec2f size);

			virtual void SetDepth(float depth);
			virtual void SetLocalAlignment(Align align);
			virtual void SetAnchorAlignment(Align align);
			virtual void SetAnchor(UIElement* anchor);
			virtual void SetActiveFlag(bool active);
			virtual void SetSolidFlag(bool solid);
			virtual void SetClipElement(UIElement* clipElement);

			inline String GetName() const { return name; }
			inline Vec2f GetPos() const { return pos; }
			inline Vec2f GetSize() const { return size; }

			inline Vec2f GetViewportPos() const { return viewportPos;}

			inline float GetDepth() const { return depth; }
			inline Align GetLocalAlignment() const { return localAlignment; }
			inline Align GetAnchorAlignment() const { return anchorAlignment; }
			inline UIElement* GetAnchor() const { return anchor; }
			inline UIElement* GetClipElement() const { return clipElement; }

			inline Array<UIElement*> GetTiedElements() const { return tiedElements; }
			inline UIElement* GetTiedParent() const { return tiedParent; }
			inline bool IsTied() const { return tiedParent != nullptr; }

			Rectf GetClipRect() const;
			
			bool IsSolid() const;			
			bool IsActive() const;

			inline UIManager* GetManager() const { return manager; }
			inline UIScene* GetScene() const { return scene; }
			inline StringView GetLayerName() const { return layer; }

			const uint GetTypeIndex() const { return typeIndex; }

			void SetProperties(const UIElementProperties&);

			using ManagerType = UIBaseElementManager;
			static constexpr const char* typeName = "UIElement";

			friend class UIManager;
			friend class UIScene;
		};
	}
}