#pragma once
#include "BlazeEngine/UI/Core/Node.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/StaticTextRenderUnit.h"
#include "BlazeEngine/UI/Graphics/RenderNode.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_API Label : public Node, public RenderNode
	{
	public:
		Label();
		Label(Node& parent, const NodeTransform& transform);
		Label(Node& parent, const NodeTransform& transform, u8StringView string, const FontAtlas& atlas, ColorRGBAf color = 0xf5f5f5ff, float fontSize = 0.0f);
		~Label();

		void ClearText();
		void BuildText(u8StringView string, const FontAtlas& atlas, float fontSize = 0.0f);
		void BuildWrappedText(u8StringView string, const FontAtlas& atlas, float wrapWidth, float fontSize = 0.0f);
		void SetColor(ColorRGBAf color);
		inline ColorRGBAf GetColor() const { return renderUnit.color; }
		
		void SetBlocksHitTestFlag(bool blocksHitTest);
		inline bool GetBlocksHitTestFlag(bool blocksHitTest) { return blocksHitTest; }

		void PreRender(const RenderContext& renderContext) override;
		RenderUnitBase* GetRenderUnit(uintMem index) override;

		HitStatus HitTest(Vec2f screenPosition) override;
	private:
		StaticTextRenderUnit renderUnit;
		Vec2f textSize;
		bool renderDataDirty;
		bool blocksHitTest;

		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
	};
}