#include "Element.h"
#include "Scene.h"

bool ConvertToColor(StringView& s, ColorRGBAf& c)
{
	String t;
	if (StringParsing::RemoveBrackets(s, t))
	{
		auto arr = StringParsing::Split(t, ',');

		String::ConvertTo(arr[0], c.r);
		String::ConvertTo(arr[1], c.g);
		String::ConvertTo(arr[2], c.b);

		if (arr.Count() == 4)
			String::ConvertTo(arr[3], c.a);
		else
			c.a = 1.0f;

		if (c.r > 1 || c.g > 1 || c.b > 1 || c.a > 1)
		{
			c.r /= 255;
			c.g /= 255;
			c.b /= 255;
			c.a /= 255;
		}
	}
	else
	{
		t = s.SubString(2, s.Size() - 2);
		uint64 v;
		StringParsing::ConvertTo(t, v, 16);
		c = v;
	}

	return true;
}

namespace Blaze::UI2
{
	Align StringToAlign(String string)
	{
		if (string == "Top")			return Align::Top;
		if (string == "TopLeft")		return Align::TopLeft;
		if (string == "TopRight")		return Align::TopRight;
		if (string == "Left")			return Align::Left;
		if (string == "Center")			return Align::Center;
		if (string == "Right")			return Align::Right;
		if (string == "BottomLeft")		return Align::BottomLeft;
		if (string == "Bottom")			return Align::Bottom;
		if (string == "BottomRight")	return Align::BottomRight;
	}

	bool ElementManager::ParseProperty(Element* el, void*, StringView name, StringView value)
	{
		if (name == "name")
		{
			StringParsing::RemoveQuotes(value, el->name);
		}
		else if (name == "offset")
		{
			String x, y;
			StringParsing::RemoveBrackets(value, x, '(', ')');
			StringParsing::SplitAtFirst(x, x, y, ',');

			String::ConvertTo(x, el->offset.x);
			String::ConvertTo(y, el->offset.y);
		}
		else if (name == "size")
		{
			String x, y;
			StringParsing::RemoveBrackets(value, x, '(', ')');
			StringParsing::SplitAtFirst(x, x, y, ',');

			String::ConvertTo(x, el->size.x);
			String::ConvertTo(y, el->size.y);
		}
		else if (name == "width")
		{
			String::ConvertTo(value, el->size.x);
		}
		else if (name == "height")
		{
			String::ConvertTo(value, el->size.y);
		}
		else if (name == "align")
		{
			String t;
			StringParsing::RemoveSpace(value, t);
			if (t[0] == '(')
			{
				String anchor, local;
				StringParsing::RemoveBrackets(t, t, '(', ')');
				StringParsing::SplitAtFirst(t, anchor, local, ',');

				el->anchorAlign = StringToAlign(anchor);
				el->localAlign = StringToAlign(local);
			}
			else
			{
				el->anchorAlign = el->localAlign = StringToAlign(t);
			}
		}
		else if (name == "active")
		{
			if (value == "true")
				el->active = true;
			else if (value == "false")
				el->active = false;
		}
		else
			return false;

		return true;
	}

	void ElementManager::Render(Element* element, void* ptr)
	{

	}

	struct PanelVertex
	{
		Vec3f pos;
		Vec4f rect;
		Vec4f fillColor;
		Vec4f borderColor;
		float radius;
		float borderWidth;
	};

	void SetPoints(Vec2f& p, Vec2f& px, Vec2f& py, Vec2f& pxy, Rectf rect)
	{
		p = rect.pos;
		px = rect.pos + Vec2f(rect.w, 0);
		py = rect.pos + Vec2f(0, rect.h);
		pxy = rect.pos + rect.size;
	}
	void SetPoints(Vec3f& p, Vec3f& px, Vec3f& py, Vec3f& pxy, Rectf rect, float depth)
	{
		p = Vec3f(rect.pos, depth);
		px = Vec3f(rect.pos + Vec2f(rect.w, 0), depth);
		py = Vec3f(rect.pos + Vec2f(0, rect.h), depth);
		pxy = Vec3f(rect.pos + rect.size, depth);
	}

	PanelManager::PanelManager()
	{
		panelsVA.EnableVertexAttribute(0);
		panelsVA.EnableVertexAttribute(1);
		panelsVA.EnableVertexAttribute(2);
		panelsVA.EnableVertexAttribute(3);
		panelsVA.EnableVertexAttribute(4);
		panelsVA.EnableVertexAttribute(5);
		panelsVA.SetVertexAttributeFormat(0, Graphics::Core::VertexAttributeType::Float, 3, false, 0);
		panelsVA.SetVertexAttributeFormat(1, Graphics::Core::VertexAttributeType::Float, 4, false, 12);
		panelsVA.SetVertexAttributeFormat(2, Graphics::Core::VertexAttributeType::Float, 4, false, 28);
		panelsVA.SetVertexAttributeFormat(3, Graphics::Core::VertexAttributeType::Float, 4, false, 44);
		panelsVA.SetVertexAttributeFormat(4, Graphics::Core::VertexAttributeType::Float, 1, false, 60);
		panelsVA.SetVertexAttributeFormat(5, Graphics::Core::VertexAttributeType::Float, 1, false, 64);
		panelsVA.SetVertexAttributeBuffer(0, &panelsVB, sizeof(PanelVertex), 0);
		panelsVA.SetVertexAttributeBuffer(1, &panelsVB, sizeof(PanelVertex), 0);
		panelsVA.SetVertexAttributeBuffer(2, &panelsVB, sizeof(PanelVertex), 0);
		panelsVA.SetVertexAttributeBuffer(3, &panelsVB, sizeof(PanelVertex), 0);
		panelsVA.SetVertexAttributeBuffer(4, &panelsVB, sizeof(PanelVertex), 0);
		panelsVA.SetVertexAttributeBuffer(5, &panelsVB, sizeof(PanelVertex), 0);

		Graphics::Core::VertexShader vs{ "assets/default/shaders/ui_panel.vert" };
		Graphics::Core::FragmentShader fs{ "assets/default/shaders/ui_panel.frag" };

		panelsSP.LinkShaders({ &vs, &fs });
	}

	bool PanelManager::ParseProperty(Element* el, void* ptr, StringView name, StringView value)
	{
		Panel* panel = (Panel*)ptr;

		if (ElementManager::ParseProperty(el, ptr, name, value))
			return true;
		
		if (name == "borderWidth")
		{
			String::ConvertTo(value, panel->borderWidth);
		}
		else if (name == "cornerRadius")
		{
			String::ConvertTo(value, panel->cornerRadius);
		}
		else if (name == "borderColor")
		{
			ConvertToColor(value, panel->borderColor);
		}
		else if (name == "color")
		{
			ConvertToColor(value, panel->color);
		}
		else
			return false;

		return true;
		// 0xffffffff	(128, 124, 0, 250);
	}

	void PanelManager::Render(Element* element, void* ptr)
	{
		Panel* panel = (Panel*)ptr;

		PanelVertex vertices[6];

		if (!element->active)
			return;

		Vec2f pos = element->position;
		Vec2f size = element->size;
		Rectf clipRect = element->clipRect;

		Vec3f p1, p2, p3, p4;
		SetPoints(p1, p2, p3, p4, { pos, size }, 0);

		vertices[0].pos = p1;
		vertices[1].pos = p3;
		vertices[2].pos = p2;
		vertices[3].pos = p2;
		vertices[4].pos = p3;
		vertices[5].pos = p4;

		for (int i = 0; i < 6; ++i)
		{
			vertices[i].rect = Vec4f(pos, size);
			vertices[i].fillColor = (Vec4f)panel->color;
			vertices[i].borderColor = (Vec4f)panel->borderColor;
			vertices[i].radius = panel->cornerRadius;
			vertices[i].borderWidth = panel->borderWidth;
		}

		Graphics::Core::SelectProgram(&panelsSP);
		Graphics::Core::SelectVertexArray(&panelsVA);
		panelsSP.SetUniform(0, element->scene->GetProjectionMatrix());
		panelsSP.SetUniform(1, Vec4f(clipRect.pos, clipRect.size));

		panelsVB.AllocateDynamicStorage({ vertices, sizeof(PanelVertex) * 6 }, Graphics::Core::GraphicsBufferDynamicStorageHint::DynamicDraw);		

		Graphics::Core::RenderPrimitiveArray(Graphics::Core::PrimitiveType::Triangles, 0, 6);
	}
}