#include "pch.h"
#include "BlazeEngineCore/File/File.h"
#include "BlazeEngineCore/File/Stream/BufferStream.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/Resources/Font/CharacterSet.h"
#include "BlazeEngine/Resources/Font/FontGlyphRenderer.h"

#include "freetype/freetype.h"
#include "rectpack2D.h"
#include "BlazeEngine/Files/Consola.h"

namespace Blaze
{
	FT_Library GetFreeTypeLibrary();

	Font::Font()
		: ptr(nullptr)
	{
	}
	Font::Font(Font&& other) noexcept
		: ptr(other.ptr), memory(std::move(other.memory))
	{
		other.ptr = nullptr;
	}
	Font::Font(const Path& path)
		: ptr(nullptr)
	{
		Load(path);
	}
	Font::~Font()
	{
		Clear();
	}
	void Font::Load(const Path& path)
	{
		File file;

		file.Open(path, FileAccessPermission::Read);

		Load(file);
	}
	void Font::Load(ReadStream& readStream)
	{
		Clear();

		FT_Error ftError;

		//Load face
		{
			if (ptr != nullptr)
			{
				FT_Done_Face((FT_Face)ptr);
				ptr = nullptr;
			}
		
			uintMem streamSize = readStream.GetSize();
			memory.Allocate(streamSize);
			readStream.Read(memory, streamSize);

			ftError = FT_New_Memory_Face(GetFreeTypeLibrary(), (const FT_Byte*)memory.Ptr(), (FT_Long)streamSize, 0, &(FT_Face&)ptr);

			if (ftError != 0)
			{				
				memory.Clear();
				ptr = nullptr;

				BLAZE_ENGINE_ERROR("Failed to open font file!");
				return;
			}
		}

		FT_Face face = (FT_Face)ptr;

		if (!(face->face_flags & FT_FACE_FLAG_SCALABLE))
		{
			BLAZE_ENGINE_ERROR("Font file not supported, it's not a scalable font!");
			return;
		}

		FT_Select_Charmap(face, FT_ENCODING_UNICODE);
		bool kern = FT_HAS_KERNING(face);		
	}
	Vec2f Font::GetGlyphKerning(uint fontPixelHeight, UnicodeChar left, UnicodeChar right) const
	{
		if (ptr == nullptr)		
			return Vec2f();		

		FT_Face face = (FT_Face)ptr;


		uint l = FT_Get_Char_Index(face, left.Value());
		uint r = FT_Get_Char_Index(face, right.Value());
		FT_Vector vec;

		if (fontPixelHeight != 0)
		{
			FT_Set_Pixel_Sizes(face, fontPixelHeight, fontPixelHeight);
			FT_Get_Kerning(face, l, r, FT_KERNING_DEFAULT, &vec);
			return Vec2f(vec.x, vec.y) / 64.0f; //divide by 64 because value is fixed-point
		}
		else
		{
			FT_Get_Kerning(face, l, r, FT_KERNING_UNSCALED, &vec);
			return Vec2f(vec.x, vec.y) / 64.0f / face->units_per_EM; //divide by 64 because value is fixed-point
		}
	}
	bool Font::GetGlyphMetrics(uint fontPixelHeight, UnicodeChar character, GlyphMetrics& data) const
	{
		if (ptr == nullptr)		
			return false;		

		FT_Face face = (FT_Face)ptr;
		
		uint glyphIndex = FT_Get_Char_Index(face, character.Value());

		if (glyphIndex == 0)
			return false;

		float scale = 0.0f;
		if (fontPixelHeight != 0)
		{
			FT_Set_Pixel_Sizes(face, fontPixelHeight, fontPixelHeight);
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_NO_BITMAP);
			scale = 1.0f / 64.0f;
		}
		else
		{
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_NO_SCALE);
			scale = 1.0f / 64.0f / face->units_per_EM;
		}

		FT_GlyphSlot glyph = face->glyph;

		data.size = Vec2f(glyph->metrics.width, glyph->metrics.height) * scale;
		data.horizontalAdvance = glyph->metrics.horiAdvance * scale;
		data.verticalAdvance = glyph->metrics.vertAdvance * scale;
		data.offset = Vec2f(glyph->metrics.horiBearingX, glyph->metrics.horiBearingY - glyph->metrics.height) * scale;		
		return true;
	}

	Map<UnicodeChar, Bitmap> Font::CreateGlyphBitmaps(uint fontPixelHeight, const CharacterSet& characterSet, FontGlyphRenderer& glyphRenderer) const
	{
		FT_Face face = (FT_Face)ptr;

		if (face == nullptr)
			return { };

		Map<UnicodeChar, Bitmap> bitmaps;

		for (auto character : characterSet)
		{
			Bitmap bitmap = glyphRenderer.Render(*this, fontPixelHeight, character);

			if (bitmap.Empty())
				continue;

			bitmaps.Insert(character, std::move(bitmap));
		}

		return bitmaps;
	}
	bool Font::CreateAtlas(uint fontPixelHeight, const CharacterSet& characterSet, FontGlyphRenderer& glyphRenderer, const CreateAtlasCallback& createAtlasCallback, const CopyBitmapCallback& copyBitmapCallback) const
	{
		if (ptr == nullptr)
			return false;

		FT_Face face = (FT_Face)ptr;

		constexpr int glyphBitmapExtraPadding = 1;

		std::vector<rect_type> glyphAtlasRects;
		glyphAtlasRects.reserve(characterSet.Count());

		struct CharacterData
		{
			Bitmap bitmap;
			UnicodeChar character;
		};
		Array<CharacterData> characterData;
		characterData.ReserveExactly(characterSet.Count());

		auto glyphAtlasRectIt = glyphAtlasRects.begin();
		for (auto character : characterSet)
		{
			Bitmap bitmap = glyphRenderer.Render(*this, fontPixelHeight, character);

			if (bitmap.Empty())
				continue;

			Vec2u size = bitmap.GetSize() + Vec2u(glyphBitmapExtraPadding * 2);

			glyphAtlasRects.push_back(rect_type(0, 0, size.x, size.y));
			characterData.AddBack(CharacterData{ std::move(bitmap), character });
		}

		const auto result_size = PackRects(glyphAtlasRects);

		createAtlasCallback(Vec2u(result_size.w, result_size.h));
		
		uintMem i = 0;
		for (auto& characterData : characterData)
		{			
			Vec2u offset = Vec2u(glyphAtlasRects[i].x, glyphAtlasRects[i].y) + Vec2u(glyphBitmapExtraPadding);

			copyBitmapCallback(characterData.character, offset, characterData.bitmap);

			++i;
		}

		return true;
	}
	void Font::Clear()
	{
		if (ptr != nullptr)
		{
			FT_Done_Face((FT_Face)ptr);
			ptr = nullptr;
		}

		memory.Clear();
	}
	Font& Font::operator=(Font&& other) noexcept
	{
		ptr = other.ptr;
		memory = std::move(other.memory);				

		other.ptr = nullptr;
		return *this;
	}
	Font Font::LoadDefault()
	{
		BufferReadStream stream{
			(void*)consolaFile, consolaFileSize
		};		

		Font font;
		font.Load(stream);

		return font;
	}
}