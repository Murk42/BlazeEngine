#include "pch.h"
#include "BlazeEngineGraphics/UI/Elements/Text/TextRenderUnit.h"

namespace Blaze::UIGraphics
{
	//Returns true if rects intersect
	static bool RectIntersect(Rectf r1, Rectf r2)
	{
		return r1.x + r1.w > r2.x && r1.x < r2.x + r2.w &&
			r1.y + r1.h > r2.y && r1.y < r2.y + r2.h;
	}
	static float Clamp(float value, float min, float max)
	{
		if (value > max)
			return max;
		if (value < min)
			return min;
		return value;
	}
	static void InscribeRectWithUV(Rectf& rect, Rectf& uvRect, const Rectf& cullingRect)
	{
		float percentLeft = 0;
		float percentRight = 1;
		float percentDown = 0;
		float percentUp = 1;

		if (rect.pos.x < cullingRect.x)
			percentLeft = Clamp((cullingRect.x - rect.pos.x) / rect.size.x, 0, 1);
		if (rect.pos.x + rect.size.x > cullingRect.x + cullingRect.w)
			percentRight = Clamp((cullingRect.x + cullingRect.w - rect.pos.x) / rect.size.x, 0, 1);
		if (rect.pos.y < cullingRect.y)
			percentDown = Clamp((cullingRect.y - rect.pos.y) / rect.size.y, 0, 1);
		if (rect.pos.y + rect.size.y > cullingRect.y + cullingRect.h)
			percentUp = Clamp((cullingRect.y + cullingRect.h - rect.pos.y) / rect.size.y, 0, 1);

		rect.pos += Vec2f(percentLeft, percentDown) * rect.size;
		rect.size *= Vec2f(percentRight - percentLeft, percentUp - percentDown);
		uvRect.pos += Vec2f(percentLeft, percentDown) * uvRect.size;
		uvRect.size *= Vec2f(percentRight - percentLeft, percentUp - percentDown);
	}
	//Returns the rect of the culling node in transformSpace of the given transform
	static Rectf GetCullingRect(UI::Node* cullingNode, Vec2f position, float scale, float rotation, Vec2f right, Vec2f up)
	{
		if (cullingNode != nullptr)
		{
			auto finalTransform = cullingNode->GetFinalTransform();

			if (finalTransform.rotation != rotation)
				return Rectf();

			Vec2f cullingPos = finalTransform.position;
			Vec2f cullingSize = finalTransform.size;

			Vec2f cullingSpacePos = cullingPos - position;

			Rectf out;
			out.pos.x = right.DotProduct(cullingSpacePos) / scale;
			out.pos.y = up.DotProduct(cullingSpacePos) / scale;
			out.size = cullingSize / scale;
			return out;
		}
		else
			return Rectf();
	}
	static bool GetUVRect(const UnicodeChar& ch, Graphics::AtlasData* atlasData, Rectf& rect)
	{
		auto it = atlasData->uvs.Find(ch);

		if (it.IsNull())
			return false;

		rect.pos = it->value.uv1;
		rect.size = it->value.uv2 - it->value.uv1;
		return true;
	}
	
	TextRenderUnit::TextRenderUnit(UI::Node* node)
		: TextRenderUnitBase("TexturedRectRenderer"), node(node), characterIndex(0), cullingNode(nullptr), font(nullptr), fontAtlasesData(nullptr),
		layoutOptions({ }), pixelFontHeight(0), renderDataDirty(false)
	{
		node->transformUpdatedEventDispatcher.AddHandler(*this);
		node->finalTransformUpdatedEventDispatcher.AddHandler(*this);
	}
	UI::NodeFinalTransform TextRenderUnit::GetFinalTransform()
	{
		return node->GetFinalTransform();
	}
	void TextRenderUnit::BeginStream()
	{		
		CleanRenderData();

		characterIndex = 0;
		SkipCulledCharacters();
	}
	void* TextRenderUnit::Advance()
	{
		SkipCulledCharacters();

		if (characterData.Count() == characterIndex || characterRenderData[characterIndex].isCulled)
			return nullptr;

		renderData.pos = characterRenderData[characterIndex].pos;
		renderData.right = characterRenderData[characterIndex].right;
		renderData.up = characterRenderData[characterIndex].up;
		renderData.color = characterRenderData[characterIndex].color;
		renderData.uv1 = characterRenderData[characterIndex].uv1;
		renderData.uv2 = characterRenderData[characterIndex].uv2;
		++characterIndex;

		return &renderData;
	}
	bool TextRenderUnit::CleanData()
	{
		node->CleanTransform();		

		if (!dataDirty)
			return false;

		dataDirty = false;

		Array<TextLineLayoutData> lines = GetTextLineLayoutData();

		if (lines.Empty())
			lines = { TextLineLayoutData{.characters = { }, .width = 0.0f } };
		
		characterData.ReserveExactly(text.CharacterCount());
		lineData.ReserveExactly(lines.Count());

		bool setTransform = false;
		auto transform = node->GetTransform();

		float lineDistance = pixelFontHeight * layoutOptions.lineDistance;

		float maxWidth = 0.0f;
		for (auto& line : lines)
			maxWidth = std::max(maxWidth, line.width);

		bool horizontallyUnderfitted = maxWidth > transform.size.x;
		bool verticallyUnderfitted = (lines.Count() - 1) * lineDistance + pixelFontHeight;
		
		if (horizontalLayout == TextHorizontalLayout::ChangeSize)
		{
			for (auto& line : lines)
				maxWidth = std::max(maxWidth, line.width);

			textSize.x = maxWidth;
			transform.size.x = textSize.x;
			setTransform = true;
		}
		else
			textSize.x = transform.size.x;

		if (verticalLayout == TextVerticalLayout::ChangeSize)
		{
			textSize.y = lineDistance * (std::max<uintMem>(lines.Count(), 1) - 0.4f); //The top line should take up less space so we subtract 0.4 percent of the line					
			transform.size.y = textSize.y;
			setTransform = true;
		}
		else
			textSize.y = transform.size.y;

		Vec2f offset = { 0.0f, lineDistance * -0.6f };

		switch (verticalLayout)
		{
		case TextVerticalLayout::AlignTop:
			offset.y += transform.size.y;
			break;
		case TextVerticalLayout::AlignBottom:
			offset.y += textSize.y;
			break;
		default:
		case TextVerticalLayout::AlignCenter:
			offset.y += transform.size.y / 2 + textSize.y / 2;
			break;
		}

		uint characterIndex = 0;
		for (uintMem i = 0; i < lines.Count(); ++i, offset.y -= lineDistance)
		{
			auto& line = lines[i];

			float scaling = 1.0f;

			if (horizontalLayout == TextHorizontalLayout::Squish && line.width > transform.size.x)
				scaling = transform.size.x / line.width;

			switch (lineAlign)
			{
			default:
			case TextLineAlign::Left:
				break;
			case TextLineAlign::Center:
				offset.x = std::ceil(transform.size.x / 2 - line.width / 2 * scaling);
				break;
			case TextLineAlign::Right:
				offset.x = transform.size.x - line.width * scaling;
				break;
			}

			auto& lineData = *this->lineData.AddBack();
			lineData.firstCharacterIndex = characterIndex;
			lineData.characterCount = line.characters.Count();
			lineData.pos = offset;
			lineData.size = Vec2f(line.width * scaling, lineDistance);
			lineData.scaling = scaling;

			if (!line.characters.Empty() && line.characters.Last().character == '\n')
				--lineData.characterCount;

			for (uintMem j = 0; j < line.characters.Count(); ++j, ++characterIndex)
			{
				auto src = line.characters[j];

				auto& dst = *characterData.AddBack();
				dst.pos = src.pos * Vec2f(scaling, 1) + offset;
				dst.size = src.size * Vec2f(scaling, 1);
				dst.character = src.character;
				dst.lineIndex = i;
			}
		}

		if (setTransform)
			node->SetTransform(transform);

		renderDataDirty = true;
		return true;
	}
	bool TextRenderUnit::CleanRenderData()
	{
		CleanData();	
		node->CleanFinalTransform();

		if (!renderDataDirty)
			return false;

		renderDataDirty = false;

		characterRenderData.Clear();

		const auto finalTransform = node->GetFinalTransform();
		const Vec2f finalPosition = finalTransform.position;
		const float finalScale = finalTransform.scale;
		const float finalRotation = finalTransform.rotation;

		const float cos = Math::Cos(finalRotation);
		const float sin = Math::Sin(finalRotation);
		const Vec2f right = Vec2f(cos, sin);
		const Vec2f up = Vec2f(-sin, cos);

		const uint finalFontHeight = (uint)std::round(pixelFontHeight * finalScale);
		Graphics::AtlasData* const  atlasData = fontAtlasesData == nullptr ? nullptr : fontAtlasesData->GetAtlasData(finalFontHeight);

		if (atlasData == nullptr)
			return true;

		const Rectf cullingRect = GetCullingRect(cullingNode, finalPosition, finalScale, finalRotation, right, up);

		const bool axisAligned = abs(cos) == 1 || abs(sin) == 1;
		const Vec2f renderOffset = axisAligned ? Vec2f(std::ceil(finalPosition.x), std::ceil(finalPosition.y)) : finalPosition;

		characterRenderData.Resize(characterData.Count());

		this->renderData.blend = 1.0f;
		this->renderData.alpha = 1.0f;
		this->renderData.texture = &atlasData->atlas;

		auto it = characterColors.FirstIterator();
		for (uintMem i = 0; i < characterData.Count(); ++i)
		{
			auto& renderData = characterRenderData[i];
			auto& data = characterData[i];

			renderData.isCulled = false;

			Rect rect = Rectf(data.pos, data.size);


			if (cullingNode != nullptr && !RectIntersect(rect, cullingRect) || horizontalLayout == TextHorizontalLayout::Wrap && !multiline && data.lineIndex != 0)
			{
				renderData.isCulled = true;
				continue;
			}

			Rect uvRect = Rectf();

			if (!GetUVRect(data.character, atlasData, uvRect))
			{
				renderData.isCulled = true;
				continue;
			}

			if (cullingRect.size != Vec2f())
				InscribeRectWithUV(rect, uvRect, cullingRect);

			renderData.pos = renderOffset + (right * rect.pos.x + up * rect.pos.y) * finalScale;
			renderData.right = right * rect.size.x * finalScale;
			renderData.up = up * rect.size.y * finalScale;
			renderData.uv1 = uvRect.pos;
			renderData.uv2 = uvRect.pos + uvRect.size;
			renderData.color = it.IsNull() ? 0xffffffff : *it;

			if (!characterColors.Empty() && it != characterColors.LastIterator())
				++it;
		}

		renderDataUpdatedEventDispatcher.Call({ this });

		return true;		
	}
	void TextRenderUnit::SetText(const StringViewUTF8& textRenderUnit)
	{
		this->text = textRenderUnit;
		node->MarkTransformDirty();
	}
	void TextRenderUnit::SetTextColor(ColorRGBAf color)
	{
		characterColors = { color };
		renderDataDirty = true;
	}
	void TextRenderUnit::SetTextCharactersColor(const ArrayView<ColorRGBAf>& colors)
	{
		characterColors = colors;
		renderDataDirty = true;
	}
	void TextRenderUnit::SetFont(Font& font)
	{
		this->font = &font;
		fontAtlasesData = Graphics::FontAtlasesData::RetrieveFromFont(font);
		node->MarkTransformDirty();
	}
	void TextRenderUnit::SetFontHeight(uint pixelFontHeight)
	{
		this->pixelFontHeight = pixelFontHeight;
		node->MarkTransformDirty();
	}
	void TextRenderUnit::SetLayoutOptions(TextLayoutOptions layoutOptions)
	{
		this->layoutOptions = layoutOptions;
		dataDirty = true;
		node->MarkTransformDirty();
	}
	void TextRenderUnit::SetCullingNode(UI::Node* node)
	{
		this->cullingNode = node;
		renderDataDirty = true;
	}	
	const Array<TextRenderUnit::CharacterData>& TextRenderUnit::GetCharacterData() 
	{
		CleanData();
		return characterData; 
	}
	const Array<TextRenderUnit::CharacterRenderData>& TextRenderUnit::GetCharacterRenderData()
	{		
		CleanRenderData();
		return characterRenderData;
	}
	const Array<TextRenderUnit::LineData>& TextRenderUnit::GetLineData()
	{
		CleanData();
		return lineData; 
	}
	UI::NodeFinalTransform TextRenderUnit::GetCullingTransform()
	{
		if (cullingNode == nullptr)
			return UI::NodeFinalTransform();

		return cullingNode->GetFinalTransform();
	}	
	void TextRenderUnit::SkipCulledCharacters()
	{
		while (true)
		{
			if (characterData.Count() == characterIndex)
				break;

			if (!characterRenderData[characterIndex].isCulled)
				break;

			++characterIndex;
		}
	}
	Array<TextLineLayoutData> TextRenderUnit::GetTextLineLayoutData()
	{
		if (font == nullptr)
			return { };

		if (multiline)
		{
			if (horizontalLayout == TextHorizontalLayout::Wrap)
			{
				WrappedLineTextLayouter layouter;
				layouter.SetWrapWidth(node->GetTransform().size.x);
				layouter.SetText(text, font->GetMetrics(pixelFontHeight));
				return layouter.GetLines();
			}
			
			MultiLineTextLayouter layouter;
			layouter.SetText(text, font->GetMetrics(pixelFontHeight));
			return layouter.GetLines();			
		}
		
		SingleLineTextLayouter layouter;
		layouter.SetText(text, font->GetMetrics(pixelFontHeight));
		return layouter.GetLines();				
	}
	void TextRenderUnit::OnEvent(UI::Node::TransformUpdatedEvent event)
	{
		dataDirty = true;
	}
	void TextRenderUnit::OnEvent(UI::Node::FinalTransformUpdatedEvent event)
	{
		renderDataDirty = true;
	}	
}