#include "BlazeEngine/Graphics/Utility/TextVertexGenerator.h"

#include "BlazeEngine/Input/Input.h"

 
namespace Blaze::Graphics
{
	void WrapString(StringUTF8& string, FontResolution* fontResolution, size_t wrapWidth)
	{
		if (wrapWidth == 0)
			return;

		const void* it = string.begin().Ptr();
		const void* wrapPtr = nullptr;

		float width = 0;		

		while (it != string.end().Ptr())
		{
			auto unicode = UnicodeChar((const char*)it);
			char ch = unicode.Value();
	
			if (!isalnum(ch))
				wrapPtr = it;

			CharacterMetrics data = fontResolution->GetCharacterData(unicode);
			width += data.advance;

			if (width > wrapWidth)
			{
				size_t byteOffset = (char*)it - (char*)string.Buffer();
				
				size_t prefixSize = (char*)wrapPtr - (char*)string.Buffer();
				StringUTF8 prefix(string.Buffer(), prefixSize);
				size_t sufixSize = (char*)string.end().Ptr() - (char*)wrapPtr;
				StringUTF8 sufix(wrapPtr, sufixSize);
				string = prefix + "\n" + sufix;
				
				it = (char*)string.Buffer() + byteOffset + 1;
				width = 0;				
				continue;
			}

			if (ch == '\n')
				width = 0;

			it = (char*)it + unicode.UTF8Size();			
		}
	}

	DefaultTextVertexGenerator::DefaultTextVertexGenerator()
		: it(text.end()), first(true),  prev()
	{
	}

	uint DefaultTextVertexGenerator::Setup(StringViewUTF8 text, FontResolution* fontResolution)
	{
		if (text.BufferSize() != 0)
		{
			this->text = text;
			this->fontResolution = fontResolution;
			//WrapString(this->text, fontResolution, wrapWidth);

			cursor = Vec2f(0, -(float)fontResolution->GetResolution() * 0.75f);
			it = this->text.begin();
			first = true;

		}
		else
		{
			this->text = StringUTF8();
			fontResolution = nullptr;
			it = this->text.end();
			first = true;
			rowCount = 1;
		}

		return this->text.CharacterCount();
	}
	bool DefaultTextVertexGenerator::IsEnd() const
	{
		return it == text.end();
	}
	bool DefaultTextVertexGenerator::HasMoreCharacters() const
	{
		return text.CharacterCount() > 0 && it == text.end();
	}
	bool DefaultTextVertexGenerator::GetNextCharacterData(Vec2f& p1, Vec2f& p2)
	{
		if (it == text.end())
			return false;

		UnicodeChar ch = it.ToUnicode();

		if (ch.Value() == '\n')
		{
			rowCount++;
			cursor.y -= fontResolution->GetBaselineDistance();
			cursor.x = 0;
			prev = it.ToUnicode();
			first = false;
			++it;
			return false;
		}

		CharacterMetrics data = fontResolution->GetCharacterData(ch);

		Vec2f kerning = fontResolution->GetKerning(prev, ch);

		p1 = data.offset + Vec2f(std::floor(cursor.x + kerning.x), std::floor(cursor.y + kerning.y));
		p2 = p1 + data.size;		

		if (data.size.x == 0)
			p2.x += data.advance;

		cursor.x += data.advance;		

		prev = it.ToUnicode();
		first = false;
		++it;

		return true;
	}		
}