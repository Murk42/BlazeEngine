#pragma once
#include "BlazeEngine/Core/Event/EventMemberFunctionTie.h"
#include "BlazeEngine/UI/Core/Node.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/StaticTextRenderUnit.h"
#include "BlazeEngine/UI/Graphics/RenderableNode.h"
#include "BlazeEngine/UI/Text/TextStyle.h"
#include "BlazeEngine/UI/Text/TextSeparations.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_API Label : public Node, public RenderableNode
	{
	public:
		Label();
		Label(Node& parent, const NodeTransform& transform);
		Label(Node& parent, const NodeTransform& transform, const TextStyle& textStyle, u8String text = "");
		~Label();

		void Clear();
		
		void SetText(u8String newText);
		void SetTextStyle(const TextStyle& newTextStyle);
		void SetWrapWidth(float newWrapWidth);

		inline u8StringView GetText() { return text; }
		inline const TextStyle& GetTextStyle() const { return textStyle; }
		inline float GetWrapWidth() const { return wrapWidth; }
		
		void SetBlocksHitTestFlag(bool blocksHitTest);
		inline bool GetBlocksHitTestFlag(bool blocksHitTest) { return blocksHitTest; }

		bool PreRender(const Graphics::RenderContext& renderContext) override;
		RenderUnitBase* GetRenderUnit(uintMem index) override;

		HitStatus HitTest(Vec2f screenPosition) override;
	private:
		StaticTextRenderUnit renderUnit;

		u8String text;
		TextStyle textStyle;
		float wrapWidth;

		Vec2f textFinalSize;

		bool blocksHitTest : 1;		
		bool rebuildGlyphs : 1;
		bool updateRenderTopology : 1;

		void UpdateTransform() override;

		void FinalScaleUpdated(const FinalScaleUpdatedEvent&);
		EVENT_MEMBER_FUNCTION(Label, FinalScaleUpdated, finalScaleUpdatedEventDispatcher)

		void RebuildGlyphs();
	};
}