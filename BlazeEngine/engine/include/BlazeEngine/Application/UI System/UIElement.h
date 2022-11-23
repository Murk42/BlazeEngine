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

		using TriggerFunction = std::function<void()>;

		class BLAZE_API UIElement
		{
			uint typeIndex;
			UIManager* manager;
			StringView layer;
			uint64 updateState;

			UIScene* scene;
			String name;

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

			std::array<std::function<void()>, 10> triggers;
		protected:
			virtual void AttachedToManager() { }
			virtual void DetachedFromManager() { }

			Result TieElement(UIElement* element);
			Result UntieElement(UIElement* element);
		public:
			UIElement();
			~UIElement();

			UIEvent sizeChanged;

			virtual Result ChangeLayer(StringView newLayer);
			inline StringView GetLayerName() const { return layer; }
			void SetLayerTrigger(const TriggerFunction& function);

			virtual void SetName(String name);
			inline String GetName() const { return name; }
			void SetNameTrigger(const TriggerFunction& function);

			virtual void SetPos(Vec2f pos);
			inline Vec2f GetPos() const { return pos; }
			void SetPosTrigger(const TriggerFunction& function);

			virtual void SetSize(Vec2f size);
			inline Vec2f GetSize() const { return size; }
			void SetSizeTrigger(const TriggerFunction& function);

			virtual void SetAnchor(UIElement* anchor);
			inline UIElement* GetAnchor() const { return anchor; }
			void SetAnchorTrigger(const TriggerFunction& function);

			virtual void SetLocalAlignment(Align align);
			inline Align GetLocalAlignment() const { return localAlignment; }
			void SetLocalAlignmentTrigger(const TriggerFunction& function);

			virtual void SetAnchorAlignment(Align align);
			inline Align GetAnchorAlignment() const { return anchorAlignment; }
			void SetAnchorAlignmentTrigger(const TriggerFunction& function);

			virtual void SetActiveFlag(bool active);
			bool IsActive() const;
			void SetActiveFlagTrigger(const TriggerFunction& function);

			virtual void SetSolidFlag(bool solid);
			bool IsSolid() const;
			void SetSolidFlagTrigger(const TriggerFunction& function);

			inline Vec2f GetViewportPos() const { return viewportPos;}
			void SetViewportPosTrigger(const TriggerFunction& function);

			virtual void SetClipElement(UIElement* clipElement);
			inline UIElement* GetClipElement() const { return clipElement; }
			Rectf GetClipRect() const;

			inline Array<UIElement*> GetTiedElements() const { return tiedElements; }
			inline UIElement* GetTiedParent() const { return tiedParent; }
			inline bool IsTied() const { return tiedParent != nullptr; }

			virtual void SetDepth(float depth);
			inline float GetDepth() const { return depth; }

			inline UIManager* GetManager() const { return manager; }
			inline UIScene* GetScene() const { return scene; }

			const uint GetTypeIndex() const { return typeIndex; }

			void SetProperties(const UIElementProperties&);

			using ManagerType = UIBaseElementManager;
			static constexpr const char* typeName = "UIElement";

			friend class UIManager;
			friend class UIScene;
		};
	}
}