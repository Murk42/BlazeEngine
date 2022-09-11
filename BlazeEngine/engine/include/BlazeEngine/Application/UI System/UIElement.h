#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/Application/UI System/UIEvent.h"

namespace Blaze
{
	namespace UI
	{
		class UIBaseElementManager;
		class UIManager;

		class BLAZE_API UIElement
		{
			UIManager* manager;
			uint64 updateState;
			uint layer;

			Rectf alignedRect;
			Rectf rect;
			
			float depth;
			bool clickable;
			bool active;

			Align localAlignment;
			Align anchorAlignment;
			UIElement* anchor;
		protected:
			virtual void AttachedToManager() { }

		public:
			UIElement();
			~UIElement();

			UIEvent sizeChanged;

			virtual void SetPos(Vec2f pos);
			virtual void SetSize(Vec2f size);
			virtual void SetRect(Rectf rect);

			virtual void SetDepth(float depth);
			virtual void SetClickableFlag(bool clickable);
			virtual void SetLocalAlignment(Align align);
			virtual void SetAnchorAlignment(Align align);
			virtual void SetAnchor(UIElement* anchor);
			virtual void SetActiveFlag(bool active);

			inline Vec2f GetPos() const { return rect.pos; }
			inline Vec2f GetSize() const { return rect.size; }
			inline Rectf GetRect() const { return rect; }

			inline Vec2f GetAlignedPos() const { return alignedRect.pos;}
			inline Vec2f GetAlignedSize() const { return alignedRect.size; }
			inline Rectf GetAlignedRect() const { return alignedRect; }

			inline float GetDepth() const { return depth; }
			inline Align GetLocalAlignment() const { return localAlignment; }
			inline Align GetAnchorAlignment() const { return anchorAlignment; }
			inline UIElement* GetAnchor() const { return anchor; }

			inline bool IsClickable() const { return clickable; }
			inline bool IsDirty() const { return updateState == 0; }
			inline bool IsActive() const { return active; }

			inline UIManager* GetManager() const { return manager; }
			inline uint GetLayer() const { return layer; }			

			using ManagerType = UIBaseElementManager;			
			static constexpr const char* typeName = "UIElement";

			friend class UIManager;
		};		
	}
}