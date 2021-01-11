#include "BlazeEngine/Graphics/TextRenderer.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "freetype/freetype.h"
#include "Engine.h"

namespace Blaze
{	
	struct FontSize 
	{
		FontSize() = default;

		struct Character
		{
			Vec2u size;
			Vec2u offset;
			uint advance = 0;
			uint uv_offset = 0;
		} characters[128];

		Texture2D texture;
		uint height = 0;		
		uint useCount = 0;

		FontSize& operator=(FontSize&& s) noexcept
		{
			memcpy(characters, s.characters, sizeof(characters));
			texture.operator=(std::move(*(Texture2D*)&s.texture));
			height = s.height;
			useCount = s.useCount;

			return *this;
		}
	};	

	void* Font::AddSize(uint height)
	{
		FontSize& size = *new FontSize;
		sizes.emplace_back() = &size;
		size.height = height;
		size.useCount = 0;

		FT_Face face = (FT_Face)ptr;

		Bitmap bitmaps[128];
		Vec2<uint> resultSize;

		FT_Set_Pixel_Sizes(face, 0, height);

		for (int i = 0; i < 128; ++i)
		{
			FT_Load_Char(face, i, FT_LOAD_RENDER);

			size.characters[i].size = Vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows);
			size.characters[i].offset = Vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top - face->glyph->bitmap.rows);
			size.characters[i].advance = face->glyph->advance.x / 64;			

			bitmaps[i].Create(size.characters[i].size, PixelFormat::Red, face->glyph->bitmap.buffer);

			resultSize.x += face->glyph->bitmap.width;
			if (face->glyph->bitmap.rows > resultSize.y)
				resultSize.y = face->glyph->bitmap.rows;
		}

		Bitmap resultBitmap;
		resultBitmap.Create(resultSize, PixelFormat::Red);

		int offset = 0;
		for (int i = 0; i < 128; ++i)
		{
			size.characters[i].uv_offset = offset;
			resultBitmap.SetPixels(Vec2i(offset, resultSize.y - bitmaps[i].GetSize().y), bitmaps[i].GetSize(), PixelFormat::Red, bitmaps[i].GetPixels());
			offset += bitmaps[i].GetSize().x;
		}

		resultBitmap.FlipVerticaly();		

		size.texture.Create(resultBitmap);

		return &size;
	}
	void Font::RemoveSize(void* size)
	{
		sizes.erase(std::find(sizes.begin(), sizes.end(), size));
	}

	Font::Font()
		: ptr(nullptr)
	{
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

	
	TextRenderer::TextRenderer()
		: font(nullptr), fontSizePtr(nullptr), size(0, 0)
	{
	}
	TextRenderer::~TextRenderer()
	{
		FontSize* fontSize = (FontSize*)fontSizePtr;
		if (fontSize != nullptr)
		{
			--fontSize->useCount;
			if (fontSize->useCount == 0)
				font->RemoveSize(fontSize);
		}
	}	
	void TextRenderer::SetFont(Font* font, uint height)
	{		
		this->font = font;

		fontSizePtr = font->AddSize(height);
		++((FontSize*)fontSizePtr)->useCount;
	}
	void TextRenderer::SetString(StringView text)
	{						
		vertices.resize(text.Size());
		FontSize* fontSize = (FontSize*)fontSizePtr;

		int offset = 0;
		for (int i = 0; i < text.Size(); ++i)
		{
			auto& c = fontSize->characters[text[i]];
			vertices[i].GetValue<0>() = Vec2f(Vec2i(offset, 0) + c.offset);
			vertices[i].GetValue<1>() = Vec2f(Vec2i(offset, 0) + c.offset + c.size);
			vertices[i].GetValue<2>() = Vec2f(c.uv_offset, 0) / fontSize->texture.GetSize().x;
			vertices[i].GetValue<3>() = Vec2f(Vec2i(c.uv_offset, 0) + c.size) / fontSize->texture.GetSize();			
			vertices[i].GetValue<4>() = Vec4f(1, 1, 1, 1);
			offset += c.advance;
		}		

		size.x = offset;

		mesh.SetVertices(vertices.data(), vertices.size());
	}
	void TextRenderer::SetColors(const std::vector<Color>& colors)
	{
		for (int i = 0; i < (vertices.size() < colors.size() ? vertices.size() : colors.size()); ++i)
			vertices[i].GetValue<4>() = colors[i].ToVector();

		mesh.SetVertices(vertices.data(), vertices.size());
	}	

	const Texture2D* TextRenderer::GetTexture() const
	{
		if (fontSizePtr != nullptr)
			return &((FontSize*)fontSizePtr)->texture;
		return nullptr;
	}

}