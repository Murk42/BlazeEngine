#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"

#include "BlazeEngine/Input/Input.h"

#include <ctype.h>

namespace Blaze::Graphics
{
	void WrapString(StringUTF8& string, FontResolution* fontResolution, size_t wrapWidth)
	{
		if (wrapWidth == 0)
			return;

		const void* it = string.begin().Ptr();
		const void* wrapPtr;

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
		: fontResolution(0), it(text.end()), first(true), wrapWidth(0), prev()
	{
	}
	void DefaultTextVertexGenerator::Setup(StringViewUTF8 text, FontResolution* fontResolution)
	{
		if (text.BufferSize() != 0)
		{
			this->text = text;
			this->fontResolution = fontResolution;
			WrapString(this->text, fontResolution, wrapWidth);

			cursor = Vec2f();
			it = this->text.begin();
			first = true;

		}
		else
		{
			this->text = StringUTF8();
			fontResolution = nullptr;
			it = this->text.end();
			first = true;
		}
	}
	bool DefaultTextVertexGenerator::IsEnd() const
	{
		return it == text.end();
	}
	bool DefaultTextVertexGenerator::GenerateVertex(Vec2f& p1, Vec2f& p2, Vec2f& uv1, Vec2f& uv2, float& next)
	{
		if (it == text.end())
			return false;

		UnicodeChar ch = it.ToUnicode();

		if (ch.Value() == '\n')
		{
			cursor.y -= fontResolution->GetBaselineDistance();
			cursor.x = 0;
			prev = it.ToUnicode();
			first = false;
			++it;
			return false;
		}

		CharacterMetrics data = fontResolution->GetCharacterData(ch);

		Vec2f kerning = fontResolution->GetKerning(prev, ch);

		p1 = data.renderOffset + Vec2f(std::floor(cursor.x + kerning.x), std::floor(cursor.y + kerning.y));
		p2 = p1 + data.size;
		uv1 = data.uv1;
		uv2 = data.uv2;

		if (data.size.x == 0)
			p2.x += data.advance;

		cursor.x += data.advance;

		next = cursor.x;

		prev = it.ToUnicode();
		first = false;
		++it;

		return true;
	}
	uint DefaultTextVertexGenerator::GetMaxVertexCount() const
	{
		return text.CharacterCount();
	}
	void DefaultTextVertexGenerator::SetWrapWidth(size_t width)
	{
		wrapWidth = width;
	}
}