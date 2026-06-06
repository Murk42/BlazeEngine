#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Core/Resource/ResourceRef.h"
#include "BlazeEngine/Graphics/Core/RendererBase.h"

namespace Blaze::Graphics
{
	struct TexturedRectRenderData
	{
		//The RGB channels of the texture and the color are mixed using the blend paramater.
		//The A channel of the texture and the color are multiplied together with the alpha parameter.
		ResourceBaseRef texture;
		Vec2f uv1, uv2;
		Vec2f pos, right, up;
		ColorRGBAf color;
		float blend;
		float alpha;
	};

	class BLAZE_API TexturedRectRenderer : public virtual RendererBase
	{
	public:
		virtual void Render(const TexturedRectRenderData& data) = 0;

		RendererTypeID GetTypeID() const override final { return RendererBase::GetTypeIDFor<TexturedRectRenderer>(); }
	};
}