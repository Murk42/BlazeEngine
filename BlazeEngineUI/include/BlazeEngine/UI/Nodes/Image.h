#pragma once
#include "BlazeEngine/UI/Core/Node.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/ImageRenderUnit.h"
#include "BlazeEngine/UI/Graphics/RenderNode.h"

namespace Blaze::UI::Nodes
{
	enum class ImageLayout
	{
		Fit,
		Stretch,
		Fill,
		KeepWidth,
		KeepHeight
	};

	class BLAZE_API Image : public Node, public RenderNode
	{
	public:
		struct Style
		{
			ColorRGBAf blendColor = ColorRGBAf(1.0f, 1.0f, 1.0f, 1.0f);
			float blendFactor = 0.0f;
			float alphaMultiplier = 1.0f;
			ImageLayout layout;
		};

		Image();
		Image(Node& parent, const NodeTransform& transform, ResourceBaseRef texture, Vec2f uv1, Vec2f uv2, const Style& style);
		~Image();

		void SetTexture(ResourceBaseRef texture, Vec2f uv1, Vec2f uv2);
		void SetStyle(const Style& style);

		inline const ResourceBaseRef GetTexture() const { return renderUnit.texture; }
		inline void GetUVs(Vec2f& uv1, Vec2f& uv2) const { uv1 = this->uv1; uv2 = this->uv2; }
		Style GetStyle() const;

		void PreRender(const UIRenderContext& renderContext) override;
		UIRenderUnitBase* GetRenderUnit(uintMem index) override;
	private:
		ImageLayout imageLayout;
		Vec2f uv1, uv2;
		ImageRenderUnit renderUnit;
		bool renderUnitDirty;

		void OnEvent(const Node::FinalTransformUpdatedEvent& event);
	};
}