#include "Transform2D.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze
{
	Transform2D::Transform2D()
		: rect(0, 0, 100, 100), rot(0), pivot(0), scale(1), rectAlign(Align::Center), viewportAlign(Align::Center)
	{		
	}
	Transform2D::Transform2D(const Transform2D& t)
		: Transform(t), rect(t.rect), rot(t.rot), pivot(t.pivot), scale(t.scale), rectAlign(t.rectAlign), viewportAlign(t.viewportAlign)
	{
	}
	Transform2D::Transform2D(Rectf rect, Align rectAlign, Align viewportAlign, Vec2f scale, float rot, Vec2f pivot)
		: rect(rect), rot(rot), pivot(pivot), scale(scale), rectAlign(rectAlign), viewportAlign(viewportAlign)
	{

	}

	void Transform2D::UpdateMatrix()
	{
		const Vec2f conversion = Vec2f(2) / Renderer::GetViewportSize();

		Vec2f rectAlignOffset = Vec2f(0);
		Vec2f viewportAlignOffset = Vec2f(0);

		switch (rectAlign)
		{
		case Align::Left:        rectAlignOffset = Vec2f(0.0f, -0.5f); break;
		case Align::Right:       rectAlignOffset = Vec2f(-1.0f, -0.5f); break;
		case Align::Bottom:      rectAlignOffset = Vec2f(-0.5f, 0.0f); break;
		case Align::Top:         rectAlignOffset = Vec2f(-0.5f, -1.0f); break;
		case Align::BottomLeft:  rectAlignOffset = Vec2f(0.0f, 0.0f); break;
		case Align::BottomRight: rectAlignOffset = Vec2f(-1.0f, 0.0f); break;
		case Align::TopLeft:     rectAlignOffset = Vec2f(0.0f, -1.0f); break;
		case Align::TopRight:    rectAlignOffset = Vec2f(-1.0f, -1.0f); break;
		case Align::Center:      rectAlignOffset = Vec2f(-0.5f, -0.5f); break;
		}
		switch (viewportAlign)
		{
		case Align::Left:        viewportAlignOffset = Vec2f(-1.0f, 0.0f); break;
		case Align::Right:       viewportAlignOffset = Vec2f(1.0f, 0.0f); break;
		case Align::Bottom:      viewportAlignOffset = Vec2f(0.0f, -1.0f); break;
		case Align::Top:         viewportAlignOffset = Vec2f(0.0f, 1.0f); break;
		case Align::BottomLeft:  viewportAlignOffset = Vec2f(-1.0f, -1.0f); break;
		case Align::BottomRight: viewportAlignOffset = Vec2f(1.0f, -1.0f); break;
		case Align::TopLeft:     viewportAlignOffset = Vec2f(-1.0f, 1.0f); break;
		case Align::TopRight:    viewportAlignOffset = Vec2f(1.0f, 1.0f); break;
		case Align::Center:      viewportAlignOffset = Vec2f(0.0f, 0.0f); break;
		}

		Vec2f pivot(this->pivot / rect.size - rectAlignOffset);
		Vec2f scale = this->scale * rect.size * conversion;
		rectAlignOffset = rectAlignOffset * scale;
		Vec2f pos = rect.pos * conversion + rectAlignOffset + viewportAlignOffset;

		mat =
			Math::TranslationMatrix<float>({ pos.x, pos.y, 0 }) *
			Math::ScalingMatrix<float>({ scale.x, scale.y, 1 }) *
			Math::TranslationMatrix<float>({ pivot.x, pivot.y, 0 }) *
			Math::RotationMatrix(Quatf(AxisAnglef({ 0, 0, 1 }, rot))) *
			Math::TranslationMatrix<float>({ -pivot.x, -pivot.y, 0 });
	}
}