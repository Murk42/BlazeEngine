#include "pch.h"
#include "BlazeEngineGraphics/UI/Elements/Text/TextRenderUnit.h"
#include <numeric>

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
	
	static float MaxWidth(ArrayView<TextLineLayoutData> lines)
	{
		return std::max_element(lines.FirstIterator(), lines.BehindIterator(), [](const auto& a, const auto& b) { return a.width < b.width; })->width;
	}
	
	static float SumHeight(ArrayView<TextLineLayoutData> lines)
	{		
		float sum = 0;
		for (auto& line : lines)
			sum += line.verticalAdvance;
		return sum;
	}

	static uintMem FindWordStart(Array<CharacterLayoutData>& characters, uintMem i)
	{
		while (i > 0 && !isspace(characters[i].character.Value()))
			--i;

		return i;
	}

	static void WrapWords(Array<TextLineLayoutData>& lines, float wrapWidth, float wrapLineAdvance)
	{				
		for (uintMem i = 0; i < lines.Count(); ++i)
			if (lines[i].width > wrapWidth && lines[i].characters.Count() > 0)
			{								
				uintMem j = lines[i].characters.Count() - 1;
				float newLineWidth = FLT_MAX;
				for (; j != 0; --j)
				{
					newLineWidth = lines[i].characters[j].pos.x + lines[i].characters[j].size.x;
					if (newLineWidth < wrapWidth)
						break;
				}

				//j = std::max(FindWordStart(lines[i].characters, j), (uintMem)1);

				auto& newLine = *lines.AddAt(i + 1);
				auto& line = lines[i];

				newLine.characters = line.characters.Split(j + 1);
				newLine.width = newLine.characters.Last().pos.x + newLine.characters.Last().size.x - newLine.characters.First().pos.x;				
				newLine.verticalAdvance = line.verticalAdvance;				
				
				line.width = newLineWidth;
				line.verticalAdvance = wrapLineAdvance;
											
				float characterOffset = newLine.characters.First().pos.x;
				for (uint j = 0; j < newLine.characters.Count(); ++j)				
					newLine.characters[j].pos.x -= characterOffset;
			}
	}
	static void WrapCharacters(Array<TextLineLayoutData>& lines, float wrapWidth)
	{

	}	
	static void SpreadCharacters(Array<TextLineLayoutData>& lines, float width, float targetWidth)
	{

	}
	static void SpreadWords(Array<TextLineLayoutData>& lines, float width, float targetWidth)
	{

	}
	static void SquishCharactersHorizontally(Array<TextLineLayoutData>& lines, float width, float targetWidth)
	{

	}
	static void SquishCharactersVertically(Array<TextLineLayoutData>& lines, float height, float targetHeight)
	{		
		//TODO scale characters and lines
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

		auto transform = node->GetTransform();				

		Array<TextLineLayoutData> lines = GetLineLayoutData(text, font, pixelFontHeight, pixelFontHeight * layoutOptions.lineAdvance);

		Vec2f textSize = FitLinesIntoBounds(
			layoutOptions.horizontallyUnderfittedOption, 
			layoutOptions.horizontallyOverfittedOption,
			layoutOptions.verticallyUnderfittedOption,
			layoutOptions.verticallyOverfittedOption,
			lines, pixelFontHeight * layoutOptions.wrappedLineAdvance, transform.size
		);
		
		float verticalOffset = GetLinesVerticalOffset(layoutOptions.lineVerticalAlign, textSize.y, transform.size.y) - lines.First().verticalAdvance;

		characterData.ReserveExactly(text.CharacterCount());
		lineData.ReserveExactly(lines.Count());

		uint characterIndex = 0;
		for (uintMem i = 0; i < lines.Count(); ++i)
		{
			auto& line = lines[i];			

			float horizontalOffset = GetLineHorizontalOffset(layoutOptions.lineHorizontalAlign, line.width, transform.size.x);

			auto& lineData = *this->lineData.AddBack();
			lineData.firstCharacterIndex = characterIndex;
			lineData.characterCount = line.characters.Count();
			lineData.pos = Vec2f(horizontalOffset, verticalOffset);
			lineData.size = Vec2f(line.width, line.verticalAdvance);			

			if (!line.characters.Empty() && line.characters.Last().character == '\n')
				--lineData.characterCount;

			for (uintMem j = 0; j < line.characters.Count(); ++j, ++characterIndex)
			{
				const auto& src = line.characters[j];

				auto& dst = *characterData.AddBack();
				dst.pos = src.pos + Vec2f(horizontalOffset, verticalOffset);
				dst.size = src.size;
				dst.character = src.character;
				dst.lineIndex = i;
			}

			verticalOffset -= line.verticalAdvance;
		}
		
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


			if (cullingNode != nullptr && !RectIntersect(rect, cullingRect))
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
	Array<TextLineLayoutData> TextRenderUnit::GetLineLayoutData(StringViewUTF8 text, Font* font, uint pixelFontHeight, float lineAdvance)
	{	
		Array<TextLineLayoutData> lines;

		if (font != nullptr)
		{
			//TODO remove text layouters
			MultiLineTextLayouter layouter;
			layouter.SetText(text, font->GetMetrics(pixelFontHeight));
			lines = layouter.GetLines();
		}

		if (lines.Empty())
			lines = { TextLineLayoutData{.characters = { }, .width = 0, .verticalAdvance = 0 } };

		for (auto& line : lines)
		{
			line.verticalAdvance = lineAdvance;			
		}

		return lines;
	}
	Vec2f TextRenderUnit::FitLinesIntoBounds(
		TextHorizontallyUnderfittedOptions horizontallyUnderfittedOption,
		TextHorizontallyOverfittedOptions horizontallyOverfittedOption,
		TextVerticallyUnderfittedOptions verticallyUnderfittedOption,
		TextVerticallyOverfittedOptions verticallyOverfittedOption, 
		Array<TextLineLayoutData>& lines, float wrappedLineAdvance, Vec2f& boundingSize
	) {		
		Vec2f textSize;
		textSize.x = MaxWidth(lines);

		if (textSize.x > boundingSize.x)
		{
			switch (horizontallyUnderfittedOption)
			{
			case TextHorizontallyUnderfittedOptions::Nothing:				
				break;
			case TextHorizontallyUnderfittedOptions::ResizeToFit:				
				boundingSize.x = textSize.x;
				break;
			case TextHorizontallyUnderfittedOptions::CharacterWrap:
				WrapCharacters(lines, boundingSize.x);
				textSize.x = MaxWidth(lines);
				break;
			case TextHorizontallyUnderfittedOptions::WordWrap:
				WrapWords(lines, boundingSize.x, wrappedLineAdvance);
				textSize.x = MaxWidth(lines);
				break;
			case TextHorizontallyUnderfittedOptions::Squish:
				SquishCharactersHorizontally(lines, textSize.x, boundingSize.x);
				textSize.x = MaxWidth(lines);
				break;
			default:
				Debug::Logger::LogError("Blaze Engine Graphics", "Invalid TextHorizontallyUnderfittedOptions enum value. The integer value was: " + StringParsing::Convert((uint)horizontallyUnderfittedOption));
				break;
			}
		}
		else if (textSize.x < boundingSize.x)
		{
			switch (horizontallyOverfittedOption)
			{
			case TextHorizontallyOverfittedOptions::Nothing:
				break;
			case TextHorizontallyOverfittedOptions::ResizeToFit:
				boundingSize.x = textSize.x;
				break;
			case TextHorizontallyOverfittedOptions::SpreadCharacters:
				SpreadCharacters(lines, textSize.x, boundingSize.x);
				break;
			case TextHorizontallyOverfittedOptions::SpreadWords:
				SpreadWords(lines, textSize.x, boundingSize.x);
				break;
			default:
				Debug::Logger::LogError("Blaze Engine Graphics", "Invalid TextHorizontallyOverfittedOptions enum value. The integer value was: " + StringParsing::Convert((uint)horizontallyOverfittedOption));
				break;
			}
		}

		textSize.y = SumHeight(lines);

		if (textSize.y > boundingSize.y)
		{
			switch (verticallyUnderfittedOption)
			{
			case TextVerticallyUnderfittedOptions::Nothing:
				break;
			case TextVerticallyUnderfittedOptions::ResizeToFit:
				boundingSize.y = textSize.y;
				break;
			case TextVerticallyUnderfittedOptions::Squish: {
				SquishCharactersVertically(lines, textSize.y, boundingSize.y);
				textSize.y = SumHeight(lines);
				break;
			}
			default:
				Debug::Logger::LogError("Blaze Engine Graphics", "Invalid TextVerticallyUnderfittedOptions enum value. The integer value was: " + StringParsing::Convert((uint)verticallyUnderfittedOption));
				break;
			}
		}
		else if (textSize.y < boundingSize.y)
		{
			switch (verticallyOverfittedOption)
			{
			case TextVerticallyOverfittedOptions::Nothing:
				break;
			case TextVerticallyOverfittedOptions::ResizeToFit:
				boundingSize.y = textSize.y;
				break;
			case TextVerticallyOverfittedOptions::SpreadLines: {				
				break;
			}
			default:
				Debug::Logger::LogError("Blaze Engine Graphics", "Invalid TextVerticallyOverfittedOptions enum value. The integer value was: " + StringParsing::Convert((uint)verticallyOverfittedOption));
				break;
			}
		}

		return textSize;
	}
	float TextRenderUnit::GetLinesVerticalOffset(TextLineVerticalAlign align, float textHeight, float boundingHeight)
	{
		switch (align)
		{
		default:
			Debug::Logger::LogError("Blaze Engine Graphics", "Invalid TextLineVerticalAlign enum value. The integer value was: " + StringParsing::Convert((uint)align));
		case TextLineVerticalAlign::Top:
			return boundingHeight;
		case TextLineVerticalAlign::Bottom:
			return textHeight;
		case TextLineVerticalAlign::Center:
			return (boundingHeight + textHeight) / 2;
		}
	}
	float TextRenderUnit::GetLineHorizontalOffset(TextLineHorizontalAlign align, float lineWidth, float boundingWidth)
	{
		switch (align)
		{
		default:
			Debug::Logger::LogError("Blaze Engine Graphics", "Invalid TextLineHorizontalAlign enum value. The integer value was: " + StringParsing::Convert((uint)align));
		case TextLineHorizontalAlign::Left:
			return 0;			
		case TextLineHorizontalAlign::Right:
			return boundingWidth - lineWidth;			
		case TextLineHorizontalAlign::Center:
			return (boundingWidth - lineWidth) / 2;
		}
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