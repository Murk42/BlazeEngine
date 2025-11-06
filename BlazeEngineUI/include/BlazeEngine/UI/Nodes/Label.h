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
		Label(Node& parent, const NodeTransform& transform, u8StringView string, const FontAtlas& atlas, float fontSize = 0.0f);
		~Label();

		void ClearText();
		void BuildText(u8StringView string, const FontAtlas& atlas, float fontSize = 0.0f);
		void BuildWrappedText(u8StringView string, const FontAtlas& atlas, float wrapWidth, float fontSize = 0.0f);

		void PreRender(const UIRenderContext& renderContext) override;
		UIRenderUnitBase* GetRenderUnit(uintMem index) override;
	private:
		StaticTextRenderUnit renderUnit;
		Vec2f textSize;
		bool renderDataDirty;

		void TransformUpdatedEvent(const Node::TransformUpdatedEvent& event);
		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
	};
}