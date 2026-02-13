#pragma once
#include "BlazeEngine/UI/Core/Node.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/StaticTextRenderUnit.h"
#include "BlazeEngine/UI/Graphics/RenderNode.h"
#include "BlazeEngine/UI/Text/TextStyle.h"
#include "BlazeEngine/UI/Text/FontManager.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_API Label : public Node, public RenderNode
	{
	public:
		Label();
		Label(Node& parent, const NodeTransform& transform);
		Label(Node& parent, const NodeTransform& transform, const TextStyle& textColor, u8String text = "");
		~Label();

		void Clear();
		
		void SetText(u8String text);
		void SetTextStyle(const TextStyle& textStyle);
		void SetWrapWidth(float wrapWidth);

		inline u8StringView GetText() { return text; }
		inline const TextStyle& GetTextStyle() const { return textStyle; }
		
		void SetBlocksHitTestFlag(bool blocksHitTest);
		inline bool GetBlocksHitTestFlag(bool blocksHitTest) { return blocksHitTest; }

		bool PreRender(const RenderContext& renderContext) override;
		RenderUnitBase* GetRenderUnit(uintMem index) override;

		HitStatus HitTest(Vec2f screenPosition) override;
	private:
		TextStyle textStyle;
		u8String text;
		float wrapWidth;

		StaticTextRenderUnit renderUnit;
		Vec2f textBuildSize;
		bool rendererTypeIDChanged : 1;

		bool blocksHitTest : 1;		

		bool GetFontAtlasData(FontManager::FontAtlasData& fontAtlasData) const;

		void UpdateTransformSize();
		void UpdateRenderUnitTransform();

		void UpdateRenderUnit();
		
		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
	};
}