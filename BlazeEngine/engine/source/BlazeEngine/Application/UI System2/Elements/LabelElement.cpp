#include "BlazeEngine/Application/UI System2/Elements/LabelElement.h"
#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"
#include <array>

namespace Blaze::UI2::Elements
{	
	Label::Label()		
		: text("Label"), fontHeight(24), fontResolution(Graphics::GetDefaultFont().GetClosestResolution(24)), dirty(true)
	{
		ColorRGBAf colors[] = { 0xffffffff };
		renderCache.GenerateVertices(text, fontHeight, fontResolution, colors);
	}

	Label::Label(StringViewUTF8 text, float fontHeight, ColorRGBAf color)
		: text(text), fontHeight(fontHeight), colors({ color }), fontResolution(Graphics::GetDefaultFont().GetClosestResolution(24)), dirty(true)
	{
	}

	Label::Label(StringViewUTF8 text, float fontHeight, ColorRGBAf color, FontResolution* fontResolution)
		: text(text), fontHeight(fontHeight), colors({ color }), fontResolution(fontResolution), dirty(true)
	{
	}	

	Label::~Label()
	{		
	}

	void Label::SetFontHeight(float fontheight)
	{
		this->fontHeight = fontHeight;		
		dirty = true;
	}

	void Label::SetFontResolution(FontResolution* fontResolution)
	{
		this->fontResolution = fontResolution;		
		dirty = true;
	}

	void Label::SetText(StringViewUTF8 text)
	{
		this->text = text;
		dirty = true;		
	}

	void Label::SetColors(std::span<ColorRGBAf> colors)
	{
		this->colors = std::vector<ColorRGBAf>(colors.begin(), colors.end());		
	}
	void Label::SetColor(ColorRGBAf color)
	{
		colors = { color };		
	}	

	void LabelSystem::Update(ECS::Component* component)
	{
		Label& label = *component->Cast<Label>();
		auto transform = label.transform;

		if (label.fontResolution == nullptr)					
			return;		

		if (label.dirty)
		{
			label.renderCache.GenerateVertices(label.text, label.fontHeight, label.fontResolution, label.colors);
			label.dirty = false;
		}

		auto& textRenderer = Graphics::GetTextRenderer();
		textRenderer.SetProjectionMatrix(label.GetScene()->GetProjectionMatrix());		
		textRenderer.Write(transform->GetViewportPos(), label.renderCache);
	}
}


