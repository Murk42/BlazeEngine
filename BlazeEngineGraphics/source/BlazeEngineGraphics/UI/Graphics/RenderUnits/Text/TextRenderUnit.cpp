#include "pch.h"
#include "BlazeEngineCore/DataStructures/Rect.h"
#include "BlazeEngineCore/DataStructures/ArrayImpl.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"
#include <numeric>

namespace Blaze::UI
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
	static void InscribeRectWithUV(Rectf& rect, Vec2f& uv1, Vec2f& uv2, const Rectf& cullingRect)
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

		Vec2f uvSize = uv2 - uv1;
		uv1 = uv1 +  Vec2f(percentLeft, percentDown) * uvSize;
		uv2 = uv1 + Vec2f(percentRight, percentUp) * uvSize;
	}
	//Returns the rect of the culling node in transformSpace of the given transform
	static Rectf GetCullingRect(Node* cullingNode, Vec2f position, float scale, float rotation, Vec2f right, Vec2f up)
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

	static uintMem FindWordStart(Array<TextRenderUnitBase::CharacterData>& characters, uintMem i)
	{
		while (i > 0 && !isspace(characters[i].character.Value()))
			--i;
		++i;

		return i;
	}
		
	static void SpreadCharacters(Array<TextRenderUnitBase::LineData>& lineData, Array<TextRenderUnitBase::CharacterData>& characterData, float width, float targetWidth)
	{
		//TODO
	}
	static void SpreadWords(Array<TextRenderUnitBase::LineData>& lineData, Array<TextRenderUnitBase::CharacterData>& characterData, float width, float targetWidth)
	{
		//TODO
	}
	static void SquishCharactersHorizontally(Array<TextRenderUnitBase::LineData>& lineData, Array<TextRenderUnitBase::CharacterData>& characterData, float width, float targetWidth)
	{
		//TODO
	}
	static void SquishCharactersVertically(Array<TextRenderUnitBase::LineData>& lineData, Array<TextRenderUnitBase::CharacterData>& characterData, float height, float targetHeight)
	{		
		//TODO scale characters and lines
	}
	
	TextRenderUnit::TextRenderUnit(TextContainerBase& textContainer, Node* node)
		: TextRenderUnitBase(textContainer, "ColoredCharacterRenderer"), node(node), characterIndex(0), cullingNode(nullptr), fontStyle(nullptr), layoutOptions({ }), renderDataDirty(false)
	{
		node->transformUpdatedEventDispatcher.AddHandler<&TextRenderUnit::TransformUpdatedEvent>(*this);
		node->finalTransformUpdatedEventDispatcher.AddHandler<&TextRenderUnit::FinalTransformUpdatedEvent>(*this);
		textContainer.textUpdatedEventDispatcher.AddHandler<&TextRenderUnit::TextUpdatedEvent>(*this);
	}
	TextRenderUnit::~TextRenderUnit()
	{
		node->transformUpdatedEventDispatcher.RemoveHandler<&TextRenderUnit::TransformUpdatedEvent>(*this);
		node->finalTransformUpdatedEventDispatcher.RemoveHandler<&TextRenderUnit::FinalTransformUpdatedEvent>(*this);
		textContainer.textUpdatedEventDispatcher.RemoveHandler<&TextRenderUnit::TextUpdatedEvent>(*this);
	}
	NodeFinalTransform TextRenderUnit::GetFinalTransform()
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

		if (characterRenderData.Count() == characterIndex || characterRenderData[characterIndex].isCulled)
			return nullptr;

		renderData.blend = 0.0f;
		renderData.alpha = 1.0f;
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

		InitializeLineAndCharacterData();
		
		float textWidth = GetTextBoundingWidth(lineData);
		if (textWidth > transform.size.x)
			ManageHorizontalUnderfittedOptions(layoutOptions.horizontallyUnderfittedOption, layoutOptions.wrappedLineAdvance * fontStyle->fontPixelHeight, textWidth, transform.size.x, lineData, characterData);
		else
			ManageHorizontalOverfittedOptions(layoutOptions.horizontallyOverfittedOption, textWidth, transform.size.x, lineData, characterData);						

		float textHeight = GetTextBoundingHeight(lineData);
		if (textHeight > transform.size.y)
			ManageVerticalUnderfittedOptions(layoutOptions.verticallyUnderfittedOption, textHeight, transform.size.y, lineData, characterData);
		else
			ManageVerticalOverfittedOptions(layoutOptions.verticallyOverfittedOption, textHeight, transform.size.y, lineData, characterData);	
		
		float verticalOffset = GetLinesVerticalOffset(layoutOptions.lineVerticalAlign, textHeight, transform.size.y);
		

		uint characterIndex = 0;
		for (uintMem i = 0; i < lineData.Count(); ++i)
		{
			auto& line = lineData[i];			

			float horizontalOffset = GetLineHorizontalOffset(layoutOptions.lineHorizontalAlign, line.size.x, transform.size.x);

			line.pos += Vec2f(horizontalOffset, verticalOffset);						

			for (uintMem j = 0; j < line.characterCount; ++j, ++characterIndex)
			{
				auto& ch = characterData[j + line.firstCharacterIndex];				
				ch.pos += Vec2f(horizontalOffset, verticalOffset);
				ch.lineIndex = i;
			}			
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

		if (fontStyle == nullptr)
			return true;

		const auto finalTransform = node->GetFinalTransform();
		const Vec2f finalPosition = finalTransform.position;
		const float finalScale = finalTransform.scale;
		const float finalRotation = finalTransform.rotation;

		const float cos = Math::Cos(finalRotation);
		const float sin = Math::Sin(finalRotation);
		const Vec2f right = Vec2f(cos, sin);
		const Vec2f up = Vec2f(-sin, cos);

		const uint finalFontHeight = (uint)std::round(fontStyle->fontPixelHeight * finalScale);				

		const Rectf cullingRect = GetCullingRect(cullingNode, finalPosition, finalScale, finalRotation, right, up);

		const bool axisAligned = abs(cos) == 1 || abs(sin) == 1;
		const Vec2f renderOffset = axisAligned ? Vec2f(std::ceil(finalPosition.x), std::ceil(finalPosition.y)) : finalPosition;

		characterRenderData.Resize(characterData.Count());

		this->renderData.blend = 1.0f;
		this->renderData.alpha = 1.0f;
		this->renderData.texture = &fontStyle->atlas.GetTexture();

		auto it = characterColors.FirstIterator();
		for (uintMem i = 0; i < characterData.Count(); ++i)
		{
			auto& renderData = characterRenderData[i];
			auto& data = characterData[i];
						
			renderData.isCulled = false;

			Rectf rect = Rectf(data.pos, data.size);
			rect.x = std::round(rect.x);
			rect.y = std::round(rect.y);
			rect.w = std::round(rect.w);
			rect.h = std::round(rect.h);

			if (cullingNode != nullptr && !RectIntersect(rect, cullingRect))
			{
				renderData.isCulled = true;
				continue;
			}

			Vec2f uv1, uv2;

			if (!fontStyle->atlas.GetCharacterUV(data.character, uv1, uv2))
			{
				renderData.isCulled = true;
				continue;
			}

			if (cullingRect.size != Vec2f())
				InscribeRectWithUV(rect, uv1, uv2, cullingRect);

			renderData.pos = renderOffset + (right * rect.pos.x + up * rect.pos.y) * finalScale;
			renderData.right = right * rect.size.x * finalScale;
			renderData.up = up * rect.size.y * finalScale;
			renderData.uv1 = uv1;
			renderData.uv2 = uv2;
			renderData.color = it.IsNull() ? 0xffffffff : *it;			

			if (!characterColors.Empty() && it != characterColors.LastIterator())
				++it;
		}

		renderDataUpdatedEventDispatcher.Call({ this });

		return true;		
	}
	void TextRenderUnit::SetTextColor(ColorRGBAf color)
	{
		characterColors = Array<ColorRGBAf>{ color };
		renderDataDirty = true;
	}
	void TextRenderUnit::SetTextCharactersColor(const ArrayView<ColorRGBAf>& colors)
	{
		characterColors = colors;
		renderDataDirty = true;
	}
	void TextRenderUnit::SetFontStyle(const FontStyle& style)
	{
		fontStyle = &style;
		node->MarkTransformDirty();
	}
	void TextRenderUnit::SetLayoutOptions(TextLayoutOptions layoutOptions)
	{
		this->layoutOptions = layoutOptions;
		dataDirty = true;
		node->MarkTransformDirty();
	}
	void TextRenderUnit::SetCullingNode(Node* node)
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
	NodeFinalTransform TextRenderUnit::GetCullingTransform()
	{
		if (cullingNode == nullptr)
			return NodeFinalTransform();

		return cullingNode->GetFinalTransform();
	}	
	void TextRenderUnit::SkipCulledCharacters()
	{
		while (true)
		{
			if (characterRenderData.Count() == characterIndex)
				break;

			if (!characterRenderData[characterIndex].isCulled)
				break;

			++characterIndex;
		}
	}		
	void TextRenderUnit::InitializeLineAndCharacterData()
	{
		lineData.Clear();
		characterData.Clear();		

		if (fontStyle == nullptr)
		{
			BLAZE_ENGINE_GRAPHICS_ERROR(" Text font style not set");
			return;
		}
		
		const float lineHeight = std::round(0.9f * fontStyle->fontPixelHeight);
		const float lineAdvance = std::round(layoutOptions.lineAdvance * fontStyle->fontPixelHeight);
		const float wrappedLineAdvance = std::round(layoutOptions.wrappedLineAdvance * fontStyle->fontPixelHeight);

		bool hasPrevChar = false;
		UnicodeChar prevCharacter = '\0';
		LineData* line = nullptr;		
		uintMem lineIndex = 0;
		Vec2f cursor = Vec2f(0, -std::round(0.6f * fontStyle->fontPixelHeight));
		
		const auto transform = node->GetTransform();

		const bool wrapLines = 
			layoutOptions.horizontallyUnderfittedOption == TextHorizontallyUnderfittedOptions::WordWrapSpread ||
			layoutOptions.horizontallyUnderfittedOption == TextHorizontallyUnderfittedOptions::WordWrap || 
			layoutOptions.horizontallyUnderfittedOption == TextHorizontallyUnderfittedOptions::CharacterWrap;						
		const auto SpreadLineWords = [&](uintMem index) {
			struct Word {
				uintMem first;
				uintMem last;
				float width;				
			};

			Array<Word> words;			
			bool isWord = false;

			float sumWordWidth = 0.0f;

			uintMem i;
			for (i = line->firstCharacterIndex; i < index; ++i)
			{
				if (characterData[i].size.y != 0)
				{
					if (!isWord)
					{
						auto& word = *words.AddBack();
						word.width = characterData[i].pos.x;
						word.first = i;
						isWord = true;
					}
				}
				else
				{
					if (isWord)
					{
						auto& word = words.Last();
						word.width = characterData[i - 1].pos.x + characterData[i - 1].size.x - word.width;
						word.last = i - 1;

						sumWordWidth += word.width;
					}										

					isWord = false;
				}
			}

			if (isWord)
			{
				auto& word = words.Last();
				word.width = characterData[i - 1].pos.x + characterData[i - 1].size.x - word.width;
				word.last = i - 1;

				sumWordWidth += word.width;
			}

			if (words.Count() < 2)
				return;

			float midWordSpaceWidth = (transform.size.x - sumWordWidth) / (words.Count() - 1);

			float offset = words.First().width + midWordSpaceWidth;
			for (uintMem i = 1; i < words.Count(); ++i)
			{
				float characterOffset = std::round(offset - characterData[words[i].first].pos.x);
				for (uintMem j = words[i].first; j <= words[i].last; ++j)				
					characterData[j].pos.x += characterOffset;

				offset += words[i].width + midWordSpaceWidth;
			}
						
			for (uintMem i = 0; i < words.Count() - 1; ++i)
			{
				float sumSpaceWidth = 0.0f;
				for (uintMem j = words[i].last + 1; j < words[i + 1].first; ++j)				
					sumSpaceWidth += characterData[j].size.x;

				float midSpaceSpaceWidth = (midWordSpaceWidth - sumSpaceWidth) / (words[i + 1].first - words[i].last - 1 + 1);

				float offset = characterData[words[i].last].pos.x + characterData[words[i].last].size.x + midSpaceSpaceWidth;
				for (uintMem j = words[i].last + 1; j < words[i + 1].first; ++j)
				{
					characterData[j].pos.x = offset;
					offset += characterData[j].size.x + midSpaceSpaceWidth;
				}				
			}

			//this will cause all the spaces after all the words in the line to be mushed into the same space as the last caracter in the last word
			for (uintMem i = words.Last().last + 1; i < index; ++i)
				characterData[i].pos.x = transform.size.x - characterData[i].size.x;

			line->size.x = transform.size.x;
		};
		const auto StartNewLine = [&](uintMem firstCharacterIndex, float lineAdvance) {			
			cursor.y -= lineAdvance;
			cursor.x = 0;
			hasPrevChar = false;

			line = &*lineData.AddBack();
			line->firstCharacterIndex = firstCharacterIndex;
			line->characterCount = 0;
			line->pos = cursor;
			line->size = Vec2f(0, lineHeight);			
		};	
		auto EndLine = [&](uintMem lastCharacterIndex) {
			line->characterCount = lastCharacterIndex - line->firstCharacterIndex + 1; //+1 to include the '\n' or '\0' characters	

			++lineIndex;
			if (lastCharacterIndex != 0)
				line->size.x = characterData[lastCharacterIndex - 1].pos.x + characterData[lastCharacterIndex - 1].size.x;
			else
				line->size.x = 0;
		};
		const auto WrapLine = [&](uintMem& index) {
			if (layoutOptions.horizontallyUnderfittedOption == TextHorizontallyUnderfittedOptions::WordWrap ||
				layoutOptions.horizontallyUnderfittedOption == TextHorizontallyUnderfittedOptions::WordWrapSpread)
			{				

				if (characterData[index].size.y == 0)
					return false;
				else
				{
					uintMem originalIndex = index;

					while (index > line->firstCharacterIndex && characterData[index].size.y != 0)					
						--index;					

					if (index == line->firstCharacterIndex)					
						index = originalIndex - 1;
										
				}

			}

			if (layoutOptions.horizontallyUnderfittedOption == TextHorizontallyUnderfittedOptions::WordWrapSpread)
				SpreadLineWords(index + 1);

			EndLine(index);
			StartNewLine(index + 1, wrappedLineAdvance);

			return false;
		};
		const auto AddCharacter = [&](UnicodeChar character, uintMem& index) {

			Vec2f offset;
			Vec2f size;
			float advance;
			Font::GlyphMetrics metrics;

			bool hasMetrics = fontStyle->font.GetGlyphMetrics(fontStyle->fontPixelHeight, character, metrics);
			Vec2f kerning;

			if (hasMetrics)
			{
				offset = metrics.offset;
				size = metrics.size;
				advance = metrics.horizontalAdvance;
				
				if (hasPrevChar)
				{
					kerning = fontStyle->font.GetGlyphKerning(fontStyle->fontPixelHeight, prevCharacter, character);
					offset += kerning;
					advance += kerning.x;
				}

				if (size.x == 0)
					size.x = advance;
			}
			else
			{
				offset = Vec2f();
				size = Vec2f(std::round(0.3f * fontStyle->fontPixelHeight), 0.0f);
				advance = fontStyle->fontPixelHeight * 0.35f;
			}


			characterData[index].character = character; //Wrap lines reads the latest character so it needs to be set before the line wrapping
			characterData[index].size = size; //Wrap lines reads the latest character size so it needs to be set before the line wrapping

			bool hadPrevChar = hasPrevChar;

			if (hasMetrics && metrics.size.x > 0 && wrapLines && offset.x + size.x + cursor.x > transform.size.x)
			{
				if (!WrapLine(index))
					return;
			}			

			characterData[index].pos = cursor + offset; //Cursor could change after line wrapping so it is set after it
			characterData[index].lineIndex = lineIndex; //Line index could change after line wrapping so it is set after it
		
			cursor.x += advance; //Cursor must be increased before creating a new line so that the first character in the new line start at 0

			hasPrevChar = true; //hasPrevChar must be set to true after getting the kerning, after line wrapping, and before a new line

			if (character.Value() == '\n')
			{
				EndLine(index);
				StartNewLine(index + 1, lineAdvance);
			}

			prevCharacter = character;						
		};

		characterData.Resize(textContainer.CharacterCount() + 1);
		StartNewLine(0, 0);

		uintMem i = 0;
		for (; i != textContainer.CharacterCount(); ++i)		
			AddCharacter(textContainer.GetSubString(i, 1).First(), i);
		
		AddCharacter('\0', i);
		EndLine(textContainer.CharacterCount());
	}
	float TextRenderUnit::GetTextBoundingWidth(Array<LineData>& lineData)
	{
		if (lineData.Empty())
			return 0;

		return std::max_element(lineData.FirstIterator(), lineData.BehindIterator(), [](const auto& a, const auto& b) { return a.size.x < b.size.x; })->size.x;
	}
	float TextRenderUnit::GetTextBoundingHeight(Array<LineData>& lineData)
	{
		if (lineData.Empty())
			return 0;

		float min = FLT_MAX;
		float max = FLT_MIN;
		
		for (auto& line : lineData)
		{
			min = std::min(min, line.pos.y);
			max = std::max(max, line.pos.y + line.size.y);
		}		

		return max - min;
	}	
	void TextRenderUnit::ManageHorizontalUnderfittedOptions(TextHorizontallyUnderfittedOptions horizontallyUnderfittedOption, float wrappedLineAdvance, float& textWidth, float& boundingWidth, Array<LineData>& lineData, Array<CharacterData>& characterData)
	{		
		switch (horizontallyUnderfittedOption)
		{
		case TextHorizontallyUnderfittedOptions::WordWrapSpread:
		case TextHorizontallyUnderfittedOptions::CharacterWrap:			
		case TextHorizontallyUnderfittedOptions::WordWrap: //Wrapping already completed
		case TextHorizontallyUnderfittedOptions::Nothing:
			break;
		case TextHorizontallyUnderfittedOptions::ResizeToFit:
			boundingWidth = textWidth;
			break;			
		case TextHorizontallyUnderfittedOptions::Squish:
			SquishCharactersHorizontally(lineData, characterData, textWidth, boundingWidth);
			textWidth = GetTextBoundingWidth(lineData);
			break;
		default:
			BLAZE_ENGINE_GRAPHICS_ERROR("Invalid TextHorizontallyUnderfittedOptions enum value. The integer value was: {}", (uint)horizontallyUnderfittedOption);
			break;
		}
	}
	void TextRenderUnit::ManageHorizontalOverfittedOptions(TextHorizontallyOverfittedOptions horizontallyOverfittedOption, float& textWidth, float& boundingWidth, Array<LineData>& lineData, Array<CharacterData>& characterData)
	{
		switch (horizontallyOverfittedOption)
		{
		case TextHorizontallyOverfittedOptions::Nothing:
			break;
		case TextHorizontallyOverfittedOptions::ResizeToFit:
			boundingWidth = textWidth;
			break;
		case TextHorizontallyOverfittedOptions::SpreadCharacters:
			SpreadCharacters(lineData, characterData, textWidth, boundingWidth);
			break;
		case TextHorizontallyOverfittedOptions::SpreadWords:
			SpreadWords(lineData, characterData, textWidth, boundingWidth);
			break;
		default:
			BLAZE_ENGINE_GRAPHICS_ERROR("Invalid TextHorizontallyOverfittedOptions enum value. The integer value was: {}", (uint)horizontallyOverfittedOption);
			break;
		}
	}	
	void TextRenderUnit::ManageVerticalUnderfittedOptions(TextVerticallyUnderfittedOptions verticallyUnderfittedOption, float& textHeight, float& boundingHeight, Array<LineData>& lineData, Array<CharacterData>& characterData)
	{
		switch (verticallyUnderfittedOption)
		{
		case TextVerticallyUnderfittedOptions::Nothing:
			break;
		case TextVerticallyUnderfittedOptions::ResizeToFit:
			boundingHeight = textHeight;
			break;
		case TextVerticallyUnderfittedOptions::Squish: {
			SquishCharactersVertically(lineData, characterData, textHeight, boundingHeight);
			textHeight = GetTextBoundingHeight(lineData);
			break;
		}
		default:
			BLAZE_ENGINE_GRAPHICS_ERROR("Invalid TextVerticallyUnderfittedOptions enum value. The integer value was: {}", (uint)verticallyUnderfittedOption);
			break;
		}
	}
	void TextRenderUnit::ManageVerticalOverfittedOptions(TextVerticallyOverfittedOptions verticallyOverfittedOption, float& textHeight, float& boundingHeight, Array<LineData>& lineData, Array<CharacterData>& characterData)
	{
		switch (verticallyOverfittedOption)
		{
		case TextVerticallyOverfittedOptions::Nothing:
			break;
		case TextVerticallyOverfittedOptions::ResizeToFit:
			boundingHeight = textHeight;
			break;
		case TextVerticallyOverfittedOptions::SpreadLines:
			break;		
		default:
			BLAZE_ENGINE_GRAPHICS_ERROR("Invalid TextVerticallyOverfittedOptions enum value. The integer value was: {}", (uint)verticallyOverfittedOption);
			break;
		}
	}	
	float TextRenderUnit::GetLinesVerticalOffset(VerticalAlign align, float textHeight, float boundingHeight)
	{
		switch (align)
		{
		case VerticalAlign::Top:
			return boundingHeight;
		case VerticalAlign::Bottom:
			return textHeight;
		case VerticalAlign::Center:
			return (boundingHeight + textHeight) / 2;
		default:
			BLAZE_ENGINE_GRAPHICS_ERROR("Invalid VerticalAlign enum value. The integer value was: {}", (uint)align);
			return boundingHeight;
		}
	}
	float TextRenderUnit::GetLineHorizontalOffset(HorizontalAlign align, float lineWidth, float boundingWidth)
	{
		switch (align)
		{
		case HorizontalAlign::Left:
			return 0;			
		case HorizontalAlign::Right:
			return boundingWidth - lineWidth;			
		case HorizontalAlign::Center:
			return (boundingWidth - lineWidth) / 2;
		default:
			BLAZE_ENGINE_GRAPHICS_ERROR("Invalid HorizontalAlign enum value. The integer value was: {}", (uint)align);
			return 0;
		}
	}
	void TextRenderUnit::TextUpdatedEvent(const TextContainerBase::TextUpdatedEvent& event)
	{
		node->MarkTransformDirty();
	}
	void TextRenderUnit::TransformUpdatedEvent(const Node::TransformUpdatedEvent& event)
	{
		dataDirty = true;
	}
	void TextRenderUnit::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		renderDataDirty = true;
	}	
}