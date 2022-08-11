#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/DataStructures/Common.h"

namespace Blaze
{
	namespace UI
	{					
		class UIBaseElementManager;		

		class BLAZE_API UIElement
		{						
			uint64 updateState;						

			Rectf alignedRect;
			Rectf rect;
			
			float depth;			
			bool clickable;			

			Align localAlignment;
			Align anchorAlignment;
			UIElement* anchor;										
		public:							
			UIElement();				

			void SetPos(Vec2f pos);
			void SetSize(Vec2f size);
			void SetRect(Rectf rect);

			void SetDepth(float depth);
			void SetClickableFlag(bool clickable);
			void SetLocalAlignment(Align align);
			void SetAnchorAlignment(Align align);
			void SetAnchor(UIElement* anchor);

			inline Vec2f GetPos()	{ return rect.pos; }
			inline Vec2f GetSize() { return rect.size; }
			inline Rectf GetRect() { return rect; }

			inline Vec2f GetAlignedPos()	{ return alignedRect.pos;}
			inline Vec2f GetAlignedSize()	{ return alignedRect.size; }
			inline Rectf GetAlignedRect()	{ return alignedRect; }

			inline float GetDepth() { return depth; }
			inline bool GetClickableFlag() { return clickable; }
			inline Align GetLocalAlignment() { return localAlignment; }
			inline Align GetAnchorAlignment() { return anchorAlignment; }
			inline UIElement* GetAnchor() { return anchor; }

			inline bool IsDirty() const { return updateState == 0; }

			using ManagerType = UIBaseElementManager;
			static constexpr const char* typeName = "UIElement";

			friend class UIManager;
		};		
	}
}