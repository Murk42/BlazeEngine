#include "pch.h"
#include "BlazeEngine/Application/UI/Components/LabelComponent.h"
#include "BlazeEngine/Application/UI/Components/UITransformComponent.h"
#include "BlazeEngine/Resources/Font/Font.h"

#define TRANSFORM_DIRTY 1Ui8
#define RENDER_DIRTY 2Ui8

namespace Blaze::UI::Components
{		 
	void LabelSystem::Render(UIScene* scene, UIComponent* component)
	{
		Label& label = *(Label*)component;

		if (label.fontResolution == nullptr)
			return;		

		//auto& textRenderer = Graphics::Core::GetDefaultTextRenderer();
		//
		//Mat4f matrix = label.transform->GetMatrix() *
		//	Mat4f::ScalingMatrix(Vec3f(Vec2f(label.transform->GetFinalSize().y / label.renderCache.GetFontHeight()), 1)) *
		//	Mat4f::TranslationMatrix(Vec3f(0, label.renderCache.GetFontHeight(), 0));
		//
		//textRenderer.Render(matrix, label.renderCache);
	}
	void LabelSystem::PreTransform(UIScene* scene, UIComponent* component)
	{
		Label& label = *(Label*)component;

		if (label.state & TRANSFORM_DIRTY)
		{			
			Vec2f size = label.textLayouter.GetSize();
			float ratio = size.x / size.y;

			switch (label.sizeControl)
			{
			case Blaze::UI::Components::LabelSizeControl::WidthControlsHeight:
				label.transform->size.y = label.transform->size.x * ratio;
				break;
			case Blaze::UI::Components::LabelSizeControl::HeightControlsWidth:
				label.transform->size.x = label.transform->size.y * ratio;
				break;
			case Blaze::UI::Components::LabelSizeControl::CustomSize:
				break;
			}

			label.state &= ~(TRANSFORM_DIRTY | RENDER_DIRTY);
		}

	}

	Label::Label()		
		: text("Label"), fontResolution(nullptr), state(TRANSFORM_DIRTY | RENDER_DIRTY), sizeControl(LabelSizeControl::HeightControlsWidth)
	{						
	}
	Label::Label(const Label& other)
		: text(other.text), fontResolution(other.fontResolution), colors(other.colors), state(TRANSFORM_DIRTY | RENDER_DIRTY), sizeControl(other.sizeControl)
	{		
	}
	Label::Label(Label&& other) noexcept
		: text(std::move(other.text)), fontResolution(other.fontResolution), colors(std::move(other.colors)), state(TRANSFORM_DIRTY | RENDER_DIRTY), sizeControl(other.sizeControl)
	{		
	}
	Label::Label(StringViewUTF8 text, ArrayView<ColorRGBAf> colors, LabelSizeControl sizeControl, FontResolution* fontResolution)
		: text(text), colors(colors), fontResolution(fontResolution), state(TRANSFORM_DIRTY | RENDER_DIRTY), sizeControl(sizeControl)
	{				
	} 	
	Label::~Label()
	{				
	}	

	void Label::SetFontResolution(FontResolution* fontResolution)
	{
		this->fontResolution = fontResolution;				
		state = TRANSFORM_DIRTY | RENDER_DIRTY;
	}

	void Label::SetText(StringViewUTF8 text)
	{
		this->text = text;
		state = TRANSFORM_DIRTY | RENDER_DIRTY;
	}

	void Label::SetColors(ArrayView<ColorRGBAf> colors)
	{
		this->colors = colors;
		state = RENDER_DIRTY;
	}
	void Label::SetColor(ColorRGBAf color)
	{
		colors = { color };		
		state = RENDER_DIRTY;
	}
	void Label::SetSizeControl(LabelSizeControl control)
	{
		sizeControl = control;
		state = TRANSFORM_DIRTY;
	}
	void Label::Setup(StringViewUTF8 text, ArrayView<ColorRGBAf> colors, LabelSizeControl sizeControl, FontResolution* fontResolution)
	{
		this->text = text;		
		this->colors = colors;
		this->fontResolution = fontResolution;
		this->sizeControl = sizeControl;
		this->state = TRANSFORM_DIRTY | RENDER_DIRTY;
	}				
}


