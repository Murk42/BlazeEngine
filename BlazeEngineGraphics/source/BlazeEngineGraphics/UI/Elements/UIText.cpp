#include "pch.h"
#include "BlazeEngine/Resources/Font/TextLayouter.h"
#include "BlazeEngineGraphics/UI/Elements/UIText.h"

namespace Blaze::UI
{
	Text::Text(Node* parent, const NodeTransform& transform)
		: Node(parent, transform), font(nullptr), characterDataDirty(true), pixelFontHeight(20), cullingNode(nullptr), renderStream(this), 
		horizontalLayout(HorizontalLayout::ChangeSize), verticalLayout(VerticalLayout::ChangeSize), lineAlign(LineAlign::Center), multiline(true), 
		textColor(0xf5f5f5ff)
	{
		Node::renderStream = &renderStream;		
		preTransformUpdateEventDispatcher.AddHandler(*this);
	}
	Text::~Text()
	{		
	}
	void Text::SetText(StringUTF8 text)
	{
		if (this->text != text)
		{
			this->text = text;
			characterDataDirty = true;
			UpdateTransform();
		}
	}
	void Text::SetTextColor(ColorRGBAf color)
	{
		this->textColor = color;
		characterDataDirty = true;
		UpdateTransform();
	}
	void Text::SetTextCharactersColor(Array<ColorRGBAf> colors)
	{
		this->charactersColors = std::move(colors);
		characterDataDirty = true;
		UpdateTransform();
	}
	void Text::SetFont(Font& font)
	{
		if (this->font != &font)
		{
			this->font = &font;
			characterDataDirty = true;
			UpdateTransform();
		}
	}
	void Text::SetFontHeight(uint pixelFontHeight)
	{		
		if (this->pixelFontHeight != pixelFontHeight)
		{
			this->pixelFontHeight = pixelFontHeight;
			characterDataDirty = true;
			UpdateTransform();
		}
	}
	void Text::SetLayoutOptions(HorizontalLayout horizontalLayout, VerticalLayout verticalLayout, LineAlign lineAlign, bool multiline)
	{
		bool changed = false;
		if (this->multiline != multiline)
		{
			this->multiline = multiline;
			changed = true;
		}
		if (this->horizontalLayout != horizontalLayout)
		{
			this->horizontalLayout = horizontalLayout;
			changed = true;
		}
		if (this->verticalLayout != verticalLayout)
		{
			this->verticalLayout = verticalLayout;
			changed = true;
		}
		if (this->lineAlign != lineAlign)
		{
			this->lineAlign = lineAlign;
			changed = true;
		}
		if (changed)
		{
			characterDataDirty = true;
			UpdateTransform();
		}
	}
	void Text::GenerateCharacterData()
	{
		characterDataDirty = false;

		if (font == nullptr)
			return;
		
		if (multiline)
		{
			if (horizontalLayout == HorizontalLayout::Wrap)
			{
				WrappedLineTextLayouter layouter;
				layouter.SetWrapWidth(GetTransform().size.x);
				CopyCharacterDataFromLayouter(&layouter);
			}
			else
			{
				MultiLineTextLayouter layouter;
				CopyCharacterDataFromLayouter(&layouter);
			}
		}
		else
		{
			SingleLineTextLayouter layouter;
			CopyCharacterDataFromLayouter(&layouter);
		}				
	}

	void Text::CopyCharacterDataFromLayouter(TextLayouterBase* layouter)
	{
		characterData.ReserveExactly(text.CharacterCount());

		layouter->SetText(text, font->GetMetrics(pixelFontHeight));

		auto lines = layouter->GetLines();

		bool setTransform;
		auto transform = GetTransform();		
				
		if (horizontalLayout == HorizontalLayout::ChangeSize)
		{
			float maxWidth = 0.0f;
			for (auto& line : lines)
				maxWidth = std::max(maxWidth, line.width);

			textSize.x = maxWidth;
			transform.size.x = textSize.x;
			setTransform = true;
		}
		else
			textSize.x = transform.size.x;

		if (verticalLayout == VerticalLayout::ChangeSize)
		{
			textSize.y = pixelFontHeight * (lines.Count() - 0.4f); //The top line should take up less space so we subtract 0.4 percent of the line		
			transform.size.y = textSize.y;
			setTransform = true;
		}
		else
			textSize.y = transform.size.y;		

		Vec2f offset = { 0.0f, pixelFontHeight * -0.6f };

		switch (verticalLayout)
		{
		case Text::VerticalLayout::AlignTop:
			offset.y += transform.size.y;
			break;
		case Text::VerticalLayout::AlignBottom:
			offset.y += textSize.y;
			break;
		default:
		case Text::VerticalLayout::AlignCenter:
			offset.y += transform.size.y / 2 + textSize.y / 2;
			break;						
		}

		auto colorIt = charactersColors.FirstIterator();
		for (uintMem i = 0; i < lines.Count(); ++i, offset.y -= pixelFontHeight)
		{
			auto& line = lines[i];			

			float scaling = 1.0f;			

			if (horizontalLayout == HorizontalLayout::Squish && line.width > transform.size.x)
				scaling = transform.size.x / line.width;

			switch (lineAlign)
			{
			default:
			case Text::LineAlign::Left:
				break;
			case Text::LineAlign::Center:
				offset.x = std::ceil(transform.size.x / 2 - line.width / 2 * scaling);
				break;
			case Text::LineAlign::Right:
				offset.x = transform.size.x - line.width * scaling;
				break;
			}

			for (uintMem j = 0; j < line.characters.Count(); ++j)
			{
				auto src = line.characters[j];

				if (!(horizontalLayout == HorizontalLayout::Wrap && !multiline && offset.x + src.pos.x + src.size.x > transform.size.x))
				{					
					auto& dst = *characterData.AddBack();
					dst.pos = src.pos * Vec2f(scaling, 1) + offset;
					dst.size = src.size * Vec2f(scaling, 1);
					dst.character = src.character;
					dst.color = (colorIt == charactersColors.BehindIterator() ? textColor : *colorIt);
				}

				if (colorIt != charactersColors.BehindIterator())
					++colorIt;
			}

		}						

		if (setTransform)
			SetTransform(transform);
	}
	
	void Text::OnEvent(NodePreTransformUpdatedEvent event)
	{
		auto transform = GetTransform();

		if (characterDataDirty || 
			(horizontalLayout == HorizontalLayout::Wrap || horizontalLayout == HorizontalLayout::Squish) && transform.size.x != textSize.x ||
			verticalLayout != VerticalLayout::ChangeSize && transform.size.y != textSize.y)
			GenerateCharacterData();
		else
		{
			bool changed = false;
		
			if (horizontalLayout == HorizontalLayout::ChangeSize && transform.size.x != textSize.x)
			{
				transform.size.x = textSize.x;
				changed = true;
			}
			if (verticalLayout == VerticalLayout::ChangeSize && transform.size.y != textSize.y)
			{
				transform.size.y = textSize.y;
				changed = true;
			}

			if (changed)
				SetTransform(transform);
		}
	}
}