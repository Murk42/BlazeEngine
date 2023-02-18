#pragma once
#include "Transform.h"

namespace Blaze::UI2
{
	class TextSystem;

	class Text : public ECS::Component
	{
		StringUTF8 text;
		ColorRGBAf color;
		FontResolution* fontResolution;
		uint fontSize;

		Transform* transform;
		Graphics::TextRenderCache trc;

		void UpdateSize()
		{
			if (fontResolution != nullptr)
			{
				float scale = (float)fontSize / (float)fontResolution->GetResolution();
				transform->size = Vec2f(trc.GetTopRight().x, (float)fontResolution->GetBaselineDistance() * 0.6f) * scale;
			}
			else
				transform->size = Vec2f();
		}
		void Update()
		{
			if (fontResolution != nullptr)
				trc.GenerateVertices(text, fontResolution);

			UpdateSize();
		}
	public:
		void SetColor(ColorRGBAf color)
		{
			this->color = color;
		}
		void SetText(StringViewUTF8 text)
		{
			this->text = text;

			Update();
		}
		void SetFontResolution(FontResolution* fontResolution)
		{
			this->fontResolution = fontResolution;

			Update();
		}
		void SetFontSize(uint fontSize)
		{
			this->fontSize = fontSize;

			UpdateSize();
		}

		COMPONENT(Text, TextSystem);

		friend class System;
	};

	class TextSystem : public ECS::System
	{
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

	public:
		void Created(ECS::Component* component) override
		{
			auto text = component->Cast<Text>();

			text->transform = text->GetEntity()->GetComponent<Transform>();

			if (text->transform == nullptr)
				Logger::AddLog(BLAZE_FATAL_LOG("Blaze Engine", "This component requires a Transform component"));
		}

		void Update(const ECS::ComponentContainer& container) override
		{
			for (auto component : container)
			{
				auto text = component->Cast<Text>();
				
				if (text->text.BufferSize() != 0)
					Graphics::Write(text->trc, text->transform->position, text->fontSize, text->color);
			}
		}

		bool SetProperty(Component* component, StringView name, StringView value) override
		{
			auto* text = component->Cast<Text>();

			if (name == "text")
			{
				String t;
				StringParsing::RemoveQuotes(value, t);
				text->SetText((StringViewUTF8)t);
			}
			else if (name == "font")
			{
				String t;
				StringParsing::RemoveQuotes(value, t);
				text->SetFontResolution(GetManager()->resourceManager->GetResource<FontResolution>(t));
			}
			else if (name == "fontSize")
			{
				uint fontSize = 0;
				String::ConvertTo(value, fontSize);
				text->SetFontSize(fontSize);
			}
			else if (name == "color")
			{
				ColorRGBAf color;
				ConvertToColor(value, color);
				text->SetColor(color);
			}
			else
				return false;

			return true;
		}
	};
}