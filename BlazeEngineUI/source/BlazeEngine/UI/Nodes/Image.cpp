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
	{
		dataMap.SetTypeName("Image");
	}
	Image::Image(Node& parent, const NodeTransform& transform, const ImageStyle& style, ImageLayout layout)
		: Image()
	{
		SetTransform(transform);
		SetParent(&parent);
		SetStyle(style);
		SetLayout(layout);
	}
	Image::~Image()
	{
	}
	void Image::SetLayout(ImageLayout layout)
	{
		this->layout = layout;

		UpdateImageRenderUnit();
	}
	ImageLayout Image::GetLayout() const
	{
		return layout;
	}
	void Image::SetStyle(const ImageStyle& style)
	{
		this->style = style;
		renderUnit.style = style;
	}
	ImageStyle Image::GetStyle() const
	{		
		return style;
	}
	RenderUnitBase* Image::GetRenderUnit(uintMem index)
	{
		return index == 0 ? &renderUnit : nullptr;
	}
	void Image::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{	
		UpdateImageRenderUnit();
	}
	void Image::UpdateImageRenderUnit()
	{
		if (auto texture = style.texture.GetValue<Graphics::OpenGL::Texture2D>())
		{
			NodeFinalTransform transform = GetFinalTransform();

			Vec2f textureSize = (Vec2f)texture->GetSize();
			auto [renderPos, renderSize, renderUV1, renderUV2] = ApplyImageLayout(layout, transform, style.uv1, style.uv2, textureSize);

			renderUnit.style.uv1 = renderUV1;
			renderUnit.style.uv2 = renderUV2;

			renderUnit.pos = renderPos;
			renderUnit.right = transform.right * renderSize.x;
			renderUnit.up = transform.Up() * renderSize.y;
		}
	}
}