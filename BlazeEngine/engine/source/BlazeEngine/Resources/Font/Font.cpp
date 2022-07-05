#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Graphics/Renderer.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftbitmap.h"
#include "freetype/ftoutln.h"


#include <cassert>
#include <algorithm>
#include <string>
#include <codecvt> // for std::codecvt_utf8
#include <locale>  // for std::wstring_convert

#include "C:/Programming/Libraries/rectpack2D/include/rectpack2D/finders_interface.h"


namespace Blaze
{	
	FT_Library GetFreeTypeLibrary();

#define FT_CHECK(x) \
	if (x != 0)\
	{	\
		if (emitLogOnFail)	\
			BLAZE_ERROR_LOG("FreeType", FT_Error_String(ftError));	\
		return BLAZE_ERROR;	\
	}	
		
	struct CharacterBitmap
	{
		uint character;
		

		Vec2f renderOffset;
		float advance;

		Vec2i size;
		size_t stride;
		uint8* data;
	};


	//TODO remove ' ' from font
	void ConvertMonochrome(uint8* output, size_t outputStride, uint8* input, size_t inputStride, size_t width, size_t height)
	{						
		for (size_t y = 0; y < height; ++y)
		{
			for (size_t x = 0; x < width; ++x)
			{
				*output = ((input[x >> 3] >> (x & 7)) & 1) * 255;

				++output;
			}
			output += outputStride;
			input += inputStride;
		}
	}	

	Font::Font()
		: ptr(nullptr), resolution(0), type(FontType::Monochrome)
	{
	}
	Font::~Font()
	{
		if (ptr != nullptr)
		{
			FT_Error ftError;
			FT_Face face = (FT_Face)ptr;
			ftError = FT_Done_Face(face);
			ptr = nullptr;

			if (ftError != 0)
				BLAZE_ERROR_LOG("FreeType", FT_Error_String(ftError));
		}
	}

	bool Font::GetCharacterData(UnicodeChar character, CharacterData& data) const
	{
		auto it = characterMap.find(character.Value());
		if (it != characterMap.end())
		{
			data = it->second;
			return true;
		}
		else
		{
			return false;
		}
	}		

	Font::CharacterVertex vertices[1024];
	Font::CharacterVertex* Font::GenerateVertices(StringViewUTF8 text, uint& vertexCount, Vec2f& size, Vec2f& bottomLeft, Vec2f& topRight) const
	{	
		if (text.Size() == 0)
		{
			vertexCount = 0;
			size = Vec2f();
			return nullptr;
		}
		Vec2f texturePixel = Vec2f(1, 1) / Vec2f(texture.GetSize());
		Font::CharacterData data;

		size = Vec2f();		

		uint characterCount = 0;
		for (auto it = text.begin(); it != text.end(); ++it)		
		{
			UnicodeChar character = it.ToUnicode();			

			GetCharacterData(character, data);

			if (data.size != Vec2i(0, 0))
			{
				vertices[characterCount].p1.x = data.offset.x + size.x;
				vertices[characterCount].p1.y = data.offset.y;
				vertices[characterCount].p2 = vertices[characterCount].p1 + Vec2f(data.size);
				vertices[characterCount].uv1 = data.uv1;
				vertices[characterCount].uv2 = data.uv2;

				topRight.x = std::max(topRight.x, vertices[characterCount].p2.x);
				topRight.y = std::max(topRight.y, vertices[characterCount].p2.y);
				bottomLeft.x = std::min(bottomLeft.x, vertices[characterCount].p1.x);
				bottomLeft.y = std::min(bottomLeft.y, vertices[characterCount].p1.y);

				switch (type)
				{
				case Blaze::FontType::Monochrome:
					break;
				case Blaze::FontType::Antialiased:
					vertices[characterCount].p1 -= Vec2f(1, 1);
					vertices[characterCount].p2 += Vec2f(1, 1);
					vertices[characterCount].uv1 -= Vec2f(texturePixel.x, -texturePixel.x);
					vertices[characterCount].uv2 += Vec2f(texturePixel.x, -texturePixel.x);
					break;
				}

				vertices[characterCount].p1 += Vec2f(0.0001f);
				vertices[characterCount].p2 -= Vec2f(0.0001f);
				vertices[characterCount].uv1 += Vec2f(texturePixel.x, -texturePixel.y) * 0.005f;
				vertices[characterCount].uv2 -= Vec2f(texturePixel.x, -texturePixel.y) * 0.005f;
			}
			else
			{
				vertices[characterCount].p1.x = data.offset.x + size.x;
				vertices[characterCount].p1.y = data.offset.y;
				vertices[characterCount].p2 = vertices[characterCount].p1 + Vec2f(data.size);
				vertices[characterCount].uv1 = data.uv1;
				vertices[characterCount].uv2 = data.uv2;
			}

			vertices[characterCount].p1 /= resolution;
			vertices[characterCount].p2 /= resolution;


			size.x += data.advance;

			++characterCount;
			//if (data.size.y > size.y)
			//	size.y = data.size.y;
		}

		topRight /= resolution;
		bottomLeft /= resolution;
		size = topRight;
		//size = topRight - bottomLeft;
		//size.x += -(int)data.advance + data.size.x;		
		//size.y = topRi;

		//for (size_t i = 0; i < text.Size(); ++i)
		//{
		//	vertices[i].p1 /= size;
		//	vertices[i].p2 /= size;
		//}

		//size *= (float)height / resolution;			
		//bottomLeft *= (float)height / resolution;
		//topRight *= (float)height / resolution;
		//
		//size.y = height;
		vertexCount = characterCount;

		return vertices;
	}

	uint CountCharacters(FT_Face face)
	{
		uint characterCount = 0;
		uint glyphIndex;
		uint character;
		for (character = FT_Get_First_Char(face, &glyphIndex); glyphIndex != 0; character = FT_Get_Next_Char(face, character, &glyphIndex))
			++characterCount;
		return characterCount;
	}

	bool IsFaceSupported(FT_Face face)
	{
		return face->face_flags & FT_FACE_FLAG_SCALABLE;
	}

	int LoadFontMonochrome(uint resolution, FT_Face face, Graphics::Core::Texture2D& atlas, std::unordered_map<uint, Font::CharacterData>& characterMap, bool emitLogOnFail);
	int LoadFontAntialiased(uint resolution, FT_Face face, Graphics::Core::Texture2D& atlas, std::unordered_map<uint, Font::CharacterData>& characterMap, bool emitLogOnFail);

	int Font::Load(StringView path, FontType type, uint resolution, bool emitLogOnFail)
	{
		FT_Error ftError;
		FT_Face face;		

		//Load face
		{
			if (ptr != nullptr)
			{
				ftError = FT_Done_Face((FT_Face)ptr);
				ptr = nullptr;
				FT_CHECK(ftError);
			}

			ftError = FT_New_Face(GetFreeTypeLibrary(), path.Ptr(), 0, &face);
			FT_CHECK(ftError);
			ptr = face;
		}

		if (!IsFaceSupported(face))
		{
			if (emitLogOnFail)
				BLAZE_ERROR_LOG("Blaze Engine", "The font isnt a vector font");
			return BLAZE_ERROR;
		}

		this->type = type;
		this->resolution = resolution;

		ftError = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
		FT_CHECK(ftError);
		 
		int returned;
		switch (type)
		{
		case Blaze::FontType::Monochrome:			returned = LoadFontMonochrome	(resolution, face, texture, characterMap, emitLogOnFail); break;
		case Blaze::FontType::Antialiased:			returned = LoadFontAntialiased	(resolution, face, texture, characterMap, emitLogOnFail); break;		
		}		
		if (returned != BLAZE_OK)
			return returned;		

		baselineDistance = (float)face->size->metrics.height / (1 << 6) / resolution;

		return BLAZE_OK;
	}
	
	constexpr bool allow_flip = false;
	using spaces_type = rectpack2D::empty_spaces<allow_flip, rectpack2D::default_empty_spaces>;
	using rect_type = rectpack2D::output_rect_t<spaces_type>;

	rectpack2D::rect_wh PackRects(std::vector<rect_type>& rectangles)
	{
		const auto runtime_flipping_mode = rectpack2D::flipping_option::DISABLED;		
		const auto max_side = 4096;
		const auto discard_step = 1;

		auto report_successful = [](rect_type&) {
			return rectpack2D::callback_result::CONTINUE_PACKING;
		};

		auto report_unsuccessful = [](rect_type&) {
			return rectpack2D::callback_result::ABORT_PACKING;
		};		

		const auto result_size = rectpack2D::find_best_packing<spaces_type>(
			rectangles,
			make_finder_input(
				max_side,
				discard_step,
				report_successful,
				report_unsuccessful,
				runtime_flipping_mode
			));

		return result_size;
	}

	int LoadFontMonochrome(uint resolution, FT_Face face, Graphics::Core::Texture2D& texture, std::unordered_map<uint, Font::CharacterData>& characterMap, bool emitLogOnFail)
	{
		FT_Error ftError;
		CharacterBitmap* characterBitmaps;

		uint characterCount;		
		uint character;
		uint glyphIndex;

		ftError = FT_Set_Pixel_Sizes(face, 0, resolution);
		FT_CHECK(ftError);		

		characterCount = CountCharacters(face);
		characterBitmaps = new CharacterBitmap[characterCount];		

		character = FT_Get_First_Char(face, &glyphIndex);
		for (uint i = 0; i < characterCount; ++i)
		{
			CharacterBitmap& bitmap = characterBitmaps[i];
			ftError = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			FT_CHECK(ftError);
			
			ftError = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
			FT_CHECK(ftError);			

			bitmap.size = Vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows);
			bitmap.data = new uint8[characterBitmaps[i].size.x * characterBitmaps[i].size.y];			
			bitmap.stride = face->glyph->bitmap.width;

			ConvertMonochrome(bitmap.data, bitmap.size.x, face->glyph->bitmap.buffer, face->glyph->bitmap.pitch, bitmap.size.x, bitmap.size.y);			

			bitmap.character = character;
			bitmap.advance = float(face->glyph->metrics.horiAdvance) / 64.0f;
			bitmap.renderOffset = Vec2f(face->glyph->metrics.horiBearingX, face->glyph->metrics.horiBearingY - face->glyph->metrics.height) / 64.0f;

			character = FT_Get_Next_Char(face, character, &glyphIndex);
		}
		std::vector<rect_type> rectangles;

		rectangles.resize(characterCount);
		for (uint i = 0; i < characterCount; ++i)
		{
			rectangles[i].x = 0;
			rectangles[i].y = 0;
			rectangles[i].w = characterBitmaps[i].size.x;
			rectangles[i].h = characterBitmaps[i].size.y;
		}
		auto result_size = PackRects(rectangles);

		Vec2i textureSize = Vec2i(result_size.w, result_size.h);

		texture.Create(textureSize, Graphics::Core::TextureInternalPixelFormat::R8);

		Graphics::Core::Texture2DSettings settings;		
		settings.mag = Graphics::Core::TextureSampling::Nearest;
		settings.min = Graphics::Core::TextureSampling::Nearest;
		settings.mip = Graphics::Core::TextureSampling::Nearest;			
		settings.xWrap = Graphics::Core::TextureWrapping::ClampToEdge;
		settings.yWrap = Graphics::Core::TextureWrapping::ClampToEdge;
		settings.mipmaps = false;

		texture.SetSettings(settings);

		for (uint i = 0; i < characterCount; ++i)
		{
			CharacterBitmap& bitmap = characterBitmaps[i];
			Font::CharacterData data;

			if (bitmap.size.x != 0)
			{
				Vec2i offset = Vec2i(rectangles[i].x, rectangles[i].y);

				texture.SetPixels(offset, bitmap.size, bitmap.stride, BitmapPixelFormat::Red, BitmapPixelType::Uint8, bitmap.data);

				data.uv1 = Vec2f(offset.x, offset.y) / Vec2f(textureSize);
				data.uv2 = Vec2f(offset.x + bitmap.size.x, offset.y + bitmap.size.y) / Vec2f(textureSize);
				data.size = bitmap.size;				
			}
			else
			{
				data.uv2 = data.uv1 = Vec2f();
				data.size = Vec2i();
			}

			data.character = bitmap.character;
			data.advance = bitmap.advance;
			data.offset = Vec2i(bitmap.renderOffset);

			std::swap(data.uv1.y, data.uv2.y);

			characterMap.insert({ bitmap.character, data });
		}		

		for (uint i = 0; i < characterCount; ++i)
			delete[] characterBitmaps[i].data;
		delete[] characterBitmaps;		

		return BLAZE_OK;
	}
	int LoadFontAntialiased(uint resolution, FT_Face face, Graphics::Core::Texture2D& texture, std::unordered_map<uint, Font::CharacterData>& characterMap, bool emitLogOnFail)
	{
		FT_Error ftError;
		CharacterBitmap* characterBitmaps;

		uint characterCount;
		uint character;
		uint glyphIndex;

		ftError = FT_Set_Pixel_Sizes(face, 0, resolution);
		FT_CHECK(ftError);


		characterCount = CountCharacters(face);
		characterBitmaps = new CharacterBitmap[characterCount];

		character = FT_Get_First_Char(face, &glyphIndex);
		for (uint i = 0; i < characterCount; ++i)
		{
			CharacterBitmap& bitmap = characterBitmaps[i];
			ftError = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			FT_CHECK(ftError);
			
			ftError = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			FT_CHECK(ftError);

			bitmap.data = new uint8[face->glyph->bitmap.pitch * face->glyph->bitmap.rows];			
			bitmap.size = Vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows);
			bitmap.stride = face->glyph->bitmap.pitch;			

			memcpy(bitmap.data, face->glyph->bitmap.buffer, bitmap.stride * bitmap.size.y);

			bitmap.character = character;
			bitmap.advance = float(face->glyph->metrics.horiAdvance) / 64.0f;
			bitmap.renderOffset = Vec2f(face->glyph->metrics.horiBearingX, face->glyph->metrics.horiBearingY - face->glyph->metrics.height) / 64.0f;

			character = FT_Get_Next_Char(face, character, &glyphIndex);
		}
		
		std::vector<rect_type> rectangles;
		rectangles.resize(characterCount);

		for (uint i = 0; i < characterCount; ++i)
		{
			rectangles[i].x = 0;
			rectangles[i].y = 0;
			rectangles[i].w = characterBitmaps[i].size.x + 2;
			rectangles[i].h = characterBitmaps[i].size.y + 2;				
		}


		const auto result_size = PackRects(rectangles);

		Vec2i textureSize = Vec2i(result_size.w, result_size.h);

		texture.Create(textureSize, Graphics::Core::TextureInternalPixelFormat::R8);

		Graphics::Core::Texture2DSettings settings;		
		settings.mag = Graphics::Core::TextureSampling::Linear;
		settings.min = Graphics::Core::TextureSampling::Linear;
		settings.mip = Graphics::Core::TextureSampling::Linear;		
		settings.xWrap = Graphics::Core::TextureWrapping::ClampToEdge;
		settings.yWrap = Graphics::Core::TextureWrapping::ClampToEdge;
		settings.mipmaps = false;

		texture.SetSettings(settings);

		for (uint i = 0; i < characterCount; ++i)
		{
			CharacterBitmap& bitmap = characterBitmaps[i];
			Font::CharacterData data;

			if (bitmap.size.x != 0)
			{
				Vec2i offset = Vec2i(rectangles[i].x + 1, rectangles[i].y + 1);				

				texture.SetPixels(offset, bitmap.size, bitmap.stride, BitmapPixelFormat::Red, BitmapPixelType::Uint8, bitmap.data);
				data.uv1 = Vec2f(offset.x, offset.y) / Vec2f(textureSize);
				data.uv2 = Vec2f(offset.x + bitmap.size.x, offset.y + bitmap.size.y) / Vec2f(textureSize);
				data.size = bitmap.size;
			}
			else
			{
				data.uv2 = data.uv1 = Vec2f();
				data.size = Vec2i();
			}

			data.character = bitmap.character;
			data.advance = bitmap.advance;
			data.offset = Vec2i(bitmap.renderOffset);

			std::swap(data.uv1.y, data.uv2.y);

			characterMap.insert({ bitmap.character, data });
		}

		for (uint i = 0; i < characterCount; ++i)
			delete[] characterBitmaps[i].data;
		delete[] characterBitmaps;

		return BLAZE_OK;
	}	
		
	/* SDF generation shader
#version 330 core

in vec2 p_uv;

uniform sampler2D u_texture;
uniform float limit;

float map(float value, float min1, float max1, float min2, float max2)
{
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
	float limitSqr = limit * limit;
	vec2 texSize = textureSize(u_texture, 0);
	float pixel = texture(u_texture, p_uv).r;

	if (pixel == 1.0f)
	{
		gl_FragColor = vec4(1, 0, 0, 1);
		return;
	}

	float value = 1.0f;
	for (float y = -limit; y <= limit; y += 1.0f)
		for (float x = -limit; x <= limit; x += 1.0f)
		{
			float dist = x * x + y * y;
			if (dist <= limitSqr)
			{
				float pixel2 = texture(u_texture, p_uv + vec2(x, y) / texSize).r;

				if (pixel2 == 1.0f)
					value = min(value,  dist / limitSqr);
			}
		}


	//value = value / sum ;
	vec4 color = vec4(1.0f - sqrt(value), 0, 0, 1);
	gl_FragColor = color;
}
	*/	
}