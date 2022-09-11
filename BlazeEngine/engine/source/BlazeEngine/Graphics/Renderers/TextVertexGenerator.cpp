#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"

#include "BlazeEngine/Input/Input.h"

namespace Blaze::Graphics
{
	DefaultTextVertexGenerator::DefaultTextVertexGenerator()
		: fontResolution(0), it(text.end()), first(true)
	{
	}
	void DefaultTextVertexGenerator::Setup(StringViewUTF8 text, FontResolution* fontResolution)
	{
		if (text.BufferSize() != 0)
		{
			this->text = text;
			this->fontResolution = fontResolution;
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
	bool DefaultTextVertexGenerator::GenerateVertex(Vec2f& p1, Vec2f& p2, Vec2f& uv1, Vec2f& uv2)
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

		if (data.size == Vec2f())
		{
			cursor.x += data.advance;
			prev = it.ToUnicode();
			first = false;
			++it;
			return false;
		}

		Vec2f kerning = fontResolution->GetKerning(prev, ch);

		if (!Input::GetKeyState(Key::T).down)
			kerning = Vec2f();

		p1 = data.renderOffset + Vec2f(std::floor(cursor.x + kerning.x), std::floor(cursor.y + kerning.y));
		p2 = p1 + data.size;
		uv1 = data.uv1;
		uv2 = data.uv2;

		cursor.x += data.advance;

		prev = it.ToUnicode();
		first = false;
		++it;

		return true;
	}
	uint DefaultTextVertexGenerator::GetMaxVertexCount() const
	{
		return text.CharacterCount();
	}
}