#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Core/Resource/ResourceRef.h"
#include "BlazeEngine/Graphics/Renderers/RendererBase.h"

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

	class TexturedRectRenderer : public RendererBase
	{
	public:
		virtual void Render(const TexturedRectRenderData& data, const RenderContext& context) = 0;

		RendererTypeID GetTypeID() const { return RendererBase::GetTypeID<TexturedRectRenderer>(); }
	};
}