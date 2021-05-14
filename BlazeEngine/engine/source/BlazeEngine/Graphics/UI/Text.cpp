#include "BlazeEngine/Graphics/UI/Text.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "freetype/freetype.h"
#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze
{		
	Font::Font()
		: ptr(nullptr)
	{
	}
	Font::Font(StringView path)
		: ptr(nullptr)
	{
		Load(path);
	}
	Font::~Font()
	{
		if (ptr != nullptr)
			FT_Done_Face((FT_Face)ptr);
		for (auto& s : sizes)
			delete s;
	}

	bool Font::Load(const StringView& path)
	{
		if (ptr != nullptr)
			FT_Done_Face((FT_Face)ptr);

		FT_Face face;
		if (FT_New_Face(engine->ft_library, path.Ptr(), 0, &face) != 0)
			return false;		
		ptr = face;

		return true;
	}	

	FontSize::FontSize()
	{

	}
	FontSize::FontSize(Font* font, uint height)
	{
		SetFontAndSize(font, height);
	}

	void FontSize::SetFontAndSize(Font* font, uint height)
	{
		this->height = height;
		this->font = font;

		FT_Face face = (FT_Face)font->ptr;

		Bitmap bitmaps[128];
		Vec2<uint> resultSize;

		FT_Set_Pixel_Sizes(face, 0, height);

		for (int i = 0; i < 128; ++i)
		{
			FT_Load_Char(face, i, FT_LOAD_RENDER);

			characters[i].textureSize = Vec2u(face->glyph->bitmap.width, face->glyph->bitmap.rows);
			characters[i].positionOffset = Vec2u(face->glyph->bitmap_left, face->glyph->bitmap_top - face->glyph->bitmap.rows);
			characters[i].advance = face->glyph->advance.x / 64;

			bitmaps[i].Create(characters[i].textureSize, PixelFormat::Red, face->glyph->bitmap.buffer);

			resultSize.x += face->glyph->bitmap.width;
			if (face->glyph->bitmap.rows > resultSize.y)
				resultSize.y = face->glyph->bitmap.rows;
		}

		Bitmap resultBitmap;
		resultBitmap.Create(resultSize, PixelFormat::Red);

		int offset = 0;
		for (int i = 0; i < 128; ++i)
		{
			characters[i].textureOffset = offset;
			resultBitmap.SetPixels(Vec2i(offset, resultSize.y - bitmaps[i].GetSize().y), bitmaps[i].GetSize(), PixelFormat::Red, bitmaps[i].GetPixels());
			offset += bitmaps[i].GetSize().x;
		}

		resultBitmap.FlipVerticaly();

		texture.Create(resultBitmap);
	}

	Text::Text()
		: fontSize(nullptr)
	{
	}
	Text::~Text()
	{				
	}

	void Text::SetFontSize(FontSize* fontSize)
	{				
		this->fontSize = fontSize;
	}	

	NormalText::NormalText()
	{

	}
	NormalText::NormalText(FontSize* fontSize, StringView text)
	{
		SetFontSize(fontSize);
		SetString(text);
	}

	void NormalText::SetString(StringView text)
	{
		if (fontSize == nullptr)
		{
			Logger::AddLog(LogType::Warning, __FUNCTION__, "No FontSize has been set");
			return;
		}
		vertices.resize(text.Size());		

		Vec2u textureSize = fontSize->GetTexture().GetSize();

		transform.size = Vec2i(0);
		int offset = 0;
		for (int i = 0; i < text.Size(); ++i)
		{
			auto charData = fontSize->GetCharData(text[i]);						
			vertices[i].GetValue<0>() = Vec2f(Vec2i(offset, 0) + charData.positionOffset);
			vertices[i].GetValue<1>() = Vec2f(Vec2i(offset, 0) + charData.positionOffset + charData.textureSize);
			vertices[i].GetValue<2>() = Vec2f(Vec2i(charData.textureOffset, 0)) / textureSize.x;
			vertices[i].GetValue<3>() = Vec2f(Vec2i(charData.textureOffset, 0) + charData.textureSize) / textureSize;			
			offset += charData.advance;
			transform.size.x += charData.advance;
			transform.size.y = std::max<int>(transform.size.y, charData.textureSize.y);
		}

		string = text;

		mesh.SetVertices(vertices.data(), vertices.size());
	}
	void NormalText::Render(BaseMaterial& material, const Color& color, const Mat4f& mvp)
	{
		if (fontSize == nullptr)
		{
			Logger::AddLog(LogType::Error, __FUNCTION__, "No FontSize set");
			return;
		}
		if (material.standardProperties.MVP == nullptr || material.standardProperties.color == nullptr || material.standardProperties.texture == nullptr)
		{
			Logger::AddLog(LogType::Error, __FUNCTION__, "Text::Render requires material with MVP, color and texture property.");
			return;
		}

		*material.standardProperties.MVP = mvp;
		*material.standardProperties.color = color.ToVector();
		*material.standardProperties.texture = &fontSize->GetTexture();

		Renderer::RenderPointArray(material, mesh);
	}

	ColoredText::ColoredText()
	{

	}
	ColoredText::ColoredText(FontSize* fontSize, StringView text, const std::vector<Color>& colors)
	{
		SetFontSize(fontSize);
		SetString(text, colors);
	}


	void ColoredText::SetString(StringView text, const std::vector<Color>& colors)
	{		
		if (fontSize == nullptr)
		{
			Logger::AddLog(LogType::Warning, __FUNCTION__, "No FontSize has been set");
			return;
		}
		vertices.resize(text.Size());

		Vec2u textureSize = fontSize->GetTexture().GetSize();

		transform.size = Vec2i(0);
		int offset = 0;
		for (int i = 0; i < text.Size(); ++i)
		{
			auto charData = fontSize->GetCharData(text[i]);
			vertices[i].GetValue<0>() = Vec2f(Vec2i(offset, 0) + charData.positionOffset);
			vertices[i].GetValue<1>() = Vec2f(Vec2i(offset, 0) + charData.positionOffset + charData.textureSize);
			vertices[i].GetValue<2>() = Vec2f(Vec2i(charData.textureOffset, 0)) / textureSize.x;
			vertices[i].GetValue<3>() = Vec2f(Vec2i(charData.textureOffset, 0) + charData.textureSize) / textureSize;
			vertices[i].GetValue<4>() = i < colors.size() ? colors[i].ToVector() : Vec4f(1, 1, 1, 1);
			offset += charData.advance;
			transform.size.x += charData.advance;
			transform.size.y = std::max<int>(transform.size.y, charData.textureSize.y);
		}

		string = text;

		mesh.SetVertices(vertices.data(), vertices.size());
		/*
		if (font == nullptr)
		{
			Logger::AddLog(LogType::Warning, __FUNCTION__, "No font has been set");
			return;
		}

		vertices.resize(text.Size());
		FontSize* fontSize = (FontSize*)fontSizePtr;		

		transform.size = Vec2i(0);
		int offset = 0;
		for (int i = 0; i < text.Size(); ++i)
		{
			auto& c = fontSize->characters[text[i]];
			vertices[i].GetValue<0>() = Vec2f(Vec2i(offset, 0) + c.offset);
			vertices[i].GetValue<1>() = Vec2f(Vec2i(offset, 0) + c.offset + c.size);
			vertices[i].GetValue<2>() = Vec2f(c.uv_offset, 0) / fontSize->texture.GetSize().x;
			vertices[i].GetValue<3>() = Vec2f(Vec2i(c.uv_offset, 0) + c.size) / fontSize->texture.GetSize();						
			vertices[i].GetValue<4>() = i < colors.size() ? colors[i].ToVector() : Vec4f(1, 1, 1, 1);
			offset += c.advance;
			transform.size.x += c.advance;
			transform.size.y = std::max<int>(transform.size.y, c.size.y);
		}		

		transform.size.x = offset;

		mesh.SetVertices(vertices.data(), vertices.size());
		*/
	}
	void ColoredText::Render(BaseMaterial& material, const Mat4f& mvp)
	{
		if (fontSize == nullptr)
		{
			Logger::AddLog(LogType::Error, __FUNCTION__, "No FontSize set");
			return;
		}
		if (material.standardProperties.MVP == nullptr || material.standardProperties.texture == nullptr)
		{
			Logger::AddLog(LogType::Error, __FUNCTION__, "Text::Render requires material with MVP, color and texture property.");
			return;
		}

		*material.standardProperties.MVP = mvp;		
		*material.standardProperties.texture = &fontSize->GetTexture();

		Renderer::RenderPointArray(material, mesh);
	}	
} 