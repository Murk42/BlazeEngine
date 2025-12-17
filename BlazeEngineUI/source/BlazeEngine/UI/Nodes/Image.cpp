#include "pch.h"
#include "BlazeEngine/UI/Nodes/Image.h"

#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"

namespace Blaze::UI::Nodes
{
	struct LayoutedImage
	{
		Vec2f renderPos;
		Vec2f renderSize;
		Vec2f uv1;
		Vec2f uv2;
	};
	static LayoutedImage ApplyImageLayout(ImageLayout layout, const NodeFinalTransform& transform, Vec2f uv1, Vec2f uv2, Vec2f textureSize)
	{
		Rectf finalSourceRect{ uv1, uv2 - uv1 };

		LayoutedImage out;
		out.renderPos = transform.position;
		out.renderSize = transform.size;

		switch (layout)
		{
		case ImageLayout::Fit: {
			finalSourceRect.size *= textureSize;
			finalSourceRect.pos *= textureSize;

			float frameRatio = transform.size.y / transform.size.x;
			float textureRatio = finalSourceRect.h / finalSourceRect.w;

			if (frameRatio < textureRatio)
			{
				float newWidth = transform.size.y / textureRatio;

				out.renderPos += transform.Right() * (transform.size.x - newWidth) / 2;
				out.renderSize.x = newWidth;
			}
			else
			{
				float newHeight = textureRatio * transform.size.x;

				out.renderPos += transform.Up() * (transform.size.y - newHeight) / 2;
				out.renderSize.y = newHeight;
			}

			finalSourceRect.size /= textureSize;
			finalSourceRect.pos /= textureSize;

			break;
		}
		case ImageLayout::Fill: {
			finalSourceRect.size *= textureSize;
			finalSourceRect.pos *= textureSize;

			float frameRatio = transform.size.y / transform.size.x;
			float textureRatio = finalSourceRect.h / finalSourceRect.w;

			if (frameRatio < textureRatio)
			{
				float newHeight = frameRatio * finalSourceRect.w;

				finalSourceRect.y += (finalSourceRect.h - newHeight) / 2;
				finalSourceRect.h = newHeight;
			}
			else
			{
				float newWidth = finalSourceRect.h / frameRatio;

				finalSourceRect.x += (finalSourceRect.w - newWidth) / 2;
				finalSourceRect.w = newWidth;
			}

			finalSourceRect.size /= textureSize;
			finalSourceRect.pos /= textureSize;

			break;
		}
		case ImageLayout::KeepWidth:
			//TODO implement this
			break;
		case ImageLayout::KeepHeight:
			//TODO implement this
			break;
		case ImageLayout::Stretch:
		default:
			break;
		}

		out.uv1 = finalSourceRect.pos;
		out.uv2 = finalSourceRect.pos + finalSourceRect.size;

		return out;
	}
	Image::Image()
		: renderUnitDirty(true), imageLayout(ImageLayout::Fit), uv1(0), uv2(1)
	{
		dataMap.SetTypeName("Image");

		finalTransformUpdatedEventDispatcher.AddHandler<&Image::OnEvent>(*this);
	}
	Image::Image(Node& parent, const NodeTransform& transform, ResourceBaseRef texture, Vec2f uv1, Vec2f uv2, const Style& style)
		: Image()
	{
		SetStyle(style);
		SetTexture(texture, uv1, uv2);
		SetTransform(transform);
		SetParent(&parent);
	}
	Image::~Image()
	{
		finalTransformUpdatedEventDispatcher.RemoveHandler<&Image::OnEvent>(*this);
	}
	void Image::SetTexture(ResourceBaseRef texture, Vec2f uv1, Vec2f uv2)
	{
		renderUnit.texture = texture;
		this->uv1 = uv1;
		this->uv2 = uv2;
		renderUnitDirty = true;
	}
	void Image::SetStyle(const Style& style)
	{
		renderUnit.color = style.blendColor;
		renderUnit.blend = style.blendFactor;
		renderUnit.alpha = style.alphaMultiplier;
		imageLayout = style.layout;
		renderUnitDirty = true;
	}
	auto Image::GetStyle() const -> Style
	{
		Style style;
		style.blendColor = renderUnit.color;
		style.blendFactor = renderUnit.blend;
		style.alphaMultiplier = renderUnit.alpha;
		style.layout = imageLayout;
		return style;
	}
	void Image::PreRender(const RenderContext& renderContext)
	{
		NodeFinalTransform transform = GetFinalTransform();

		if (!renderUnitDirty)
			return;

		renderUnitDirty = false;

		Vec2f textureSize = (Vec2f)renderUnit.texture.Get<Graphics::OpenGL::Texture2D>().GetSize();
		auto [renderPos, renderSize, renderUV1, renderUV2] = ApplyImageLayout(imageLayout, transform, uv1, uv2, textureSize);

		renderUnit.uv1 = renderUV1;
		renderUnit.uv2 = renderUV2;
		renderUnit.pos = renderPos;
		renderUnit.right = transform.right * renderSize.x;
		renderUnit.up = Vec2f(-transform.right.y, transform.right.x) * renderSize.y;
	}
	RenderUnitBase* Image::GetRenderUnit(uintMem index)
	{
		return index == 0 ? &renderUnit : nullptr;
	}
	void Image::OnEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		renderUnitDirty = true;
	}
}