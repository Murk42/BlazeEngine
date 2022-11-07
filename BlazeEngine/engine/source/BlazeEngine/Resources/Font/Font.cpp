#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Graphics/Renderer.h"

#include "freetype/freetype.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include "rectpack2D.h"

namespace Blaze
{
	FT_Library GetFreeTypeLibrary();

	struct CharacterBitmap
	{
		uint character = 0;		

		Vec2i size;
		size_t stride = 0;
		uint8* data = nullptr;
	};	

	void CopyBitmapData(CharacterBitmap& dst, const FT_Bitmap& src, bool isMonochrome)
	{
		delete[] dst.data;
		
		if (isMonochrome)
		{
			dst.data = new uint8[src.width * src.rows];

			uint offset = 0;
			uint srcOffset = 0;
			for (uint y = 0; y < src.rows; ++y)
			{
				for (uint x = 0; x < src.width; ++x)				
					dst.data[offset + x] = ((src.buffer[x / 8 + srcOffset] >> (7 - x % 8)) & 1) ? 255 : 0;
					//dst.data[offset + x] = (src.buffer[x + offset]) ? 255 : 0;
				
				offset += src.width;
				srcOffset += src.pitch;
			}

			dst.size = Vec2i(src.width, src.rows);
			dst.stride = src.width;
		}
		else
		{
			dst.data = new uint8[src.pitch * src.rows];
			dst.size = Vec2i(src.width, src.rows);
			dst.stride = src.pitch;

			memcpy(dst.data, src.buffer, dst.stride * dst.size.y);
		}

	}
	void FreeBitmapData(CharacterBitmap& bm)
	{
		delete[] bm.data;
	}

	void SelectPixelFormats(Graphics::Core::TextureInternalPixelFormat& internalFormat, BitmapPixelFormat& bitmapFormat, FontResolutionRenderType renderType)
	{
		using namespace Graphics::Core;
		switch (renderType)
		{
		case Blaze::FontResolutionRenderType::Monochrome:		
			internalFormat = TextureInternalPixelFormat::R; 
			bitmapFormat = BitmapPixelFormat::Red;
			break;
		case Blaze::FontResolutionRenderType::Antialiased:		
			internalFormat = TextureInternalPixelFormat::R; 
			bitmapFormat = BitmapPixelFormat::Red;
			break;
		case Blaze::FontResolutionRenderType::HorizontalLCD:	
			internalFormat = TextureInternalPixelFormat::RGB; 
			bitmapFormat = BitmapPixelFormat::RGB;
			break;
		case Blaze::FontResolutionRenderType::VerticalLCD:		
			internalFormat = TextureInternalPixelFormat::RGB;
			bitmapFormat = BitmapPixelFormat::RGB;
			break;
		case Blaze::FontResolutionRenderType::SDF:				
			internalFormat = TextureInternalPixelFormat::R; 
			bitmapFormat = BitmapPixelFormat::Red;
			break;
		}
	}
	void SelectRenderMode(uint32& mode, FontResolutionRenderType type)
	{
		switch (type)
		{
		case Blaze::FontResolutionRenderType::Monochrome:
			mode = FT_RENDER_MODE_MONO;
			//mode = FT_LOAD_TARGET_MONO;
			break;
		case Blaze::FontResolutionRenderType::Antialiased:
			mode = FT_RENDER_MODE_NORMAL;
			//mode = FT_LOAD_TARGET_NORMAL;
			break;
		case Blaze::FontResolutionRenderType::HorizontalLCD:
			mode = FT_RENDER_MODE_LCD;
			//mode = FT_LOAD_TARGET_LCD;
			break;
		case Blaze::FontResolutionRenderType::VerticalLCD:
			mode = FT_RENDER_MODE_LCD_V;
			//mode = FT_LOAD_TARGET_LCD_V ;
			break;
		case Blaze::FontResolutionRenderType::SDF:
			mode = FT_RENDER_MODE_SDF;
			//mode = 0;
			break;		
		}
	}
	
	void CreateTexture(Graphics::Core::Texture2D& atlas, std::vector<rect_type>& rectangles, Graphics::Core::TextureInternalPixelFormat internalPixelFormat)
	{
		const auto result_size = PackRects(rectangles);
		Vec2i textureSize = Vec2i(result_size.w, result_size.h);
		//int bigger = std::max(textureSize.x, textureSize.y);
		//bigger = 1 << (int)std::ceil(log2(bigger));
		//textureSize = { bigger, bigger };
		atlas.Create(textureSize, internalPixelFormat);

		atlas.SetSettings({
			Graphics::Core::TextureWrapping::ClampToEdge,
			Graphics::Core::TextureWrapping::ClampToEdge,
			Graphics::Core::TextureSampling::Linear,
			Graphics::Core::TextureSampling::Linear,
			Graphics::Core::TextureSampling::Nearest,
			false
			});
	}

	void CopyGlypMetrics(CharacterMetrics& out, FT_GlyphSlot& in, uint pixelsPerUnit, FontResolutionRenderType renderType)
	{
		
		out.size = Vec2f(in->metrics.width, in->metrics.height) / 64.0f;
		out.advance = float(in->metrics.horiAdvance) / 64.0f;
		out.renderOffset = Vec2f(in->metrics.horiBearingX, in->metrics.horiBearingY - in->metrics.height) / 64.0f;
		//} 
		
		//float k = 1.0f / pixelsPerUnit;
		//out.size *= k;
		//out.advance *= k;
		//out.renderOffset *= k;		
	}		

	FontResolution::FontResolution()
		: font(nullptr), resolution(0), characterCount(0), baselineDistance(0), renderType((FontResolutionRenderType)-1)
	{
	}

	Result FontResolution::LoadCharacters(uint32 first, uint32 last)
	{
		FT_Face face = (FT_Face)font->ptr;

		uint32 renderMode;
		SelectRenderMode(renderMode, renderType);

		FT_Set_Pixel_Sizes(face, 0, resolution);

		int notLoadedCount = 0;
		for (uint32 ch = first; ch != last + 1; ++ch)			
			if (characterMap.find(ch) == characterMap.end())
			{
				uint glyphIndex = FT_Get_Char_Index(face, ch);

				if (glyphIndex == 0)
				{
					++notLoadedCount;
					continue;
				}

				CharacterMetrics data;
								
				FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
				CopyGlypMetrics(data, face->glyph, font->pixelsPerUnit, renderType);

				characterMap.insert({ ch, data });

				++characterCount;
			}

		return Result();
		//if (notLoadedCount == 0)
		//	return Result();
		//else
		//	return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine",
		//		"Not all characters were loaded. " + String::Convert(notLoadedCount) + " out of " + String::Convert(last - first + 1) + "."));
	}
	Result FontResolution::LoadAllCharacters()
	{
		FT_Face face = (FT_Face)font->ptr;

		uint glyphIndex;
		uint character = FT_Get_First_Char(face, &glyphIndex);

		while (glyphIndex != 0)
			if (characterMap.find(character) == characterMap.end())
			{
				CharacterMetrics data;

				FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
				CopyGlypMetrics(data, face->glyph, font->pixelsPerUnit, renderType);

				characterMap.insert({ character, data });

				character = FT_Get_Next_Char(face, character, &glyphIndex);
				++characterCount;
			}

		return Result();
	}

	Result FontResolution::CreateAtlas()
	{		
		FT_Face face = (FT_Face)font->ptr;
		CharacterBitmap* characterBitmaps = new CharacterBitmap[characterCount];		
		std::vector<rect_type> rectangles(characterCount);	
						
		//resolution = font->baselineDistance;
		//FT_Set_Char_Size(face, 0, resolution * 64, 0, 0);
		FT_Set_Pixel_Sizes(face, 0, resolution);		

		Graphics::Core::TextureInternalPixelFormat internalPixelFormat;		
		BitmapPixelFormat bitmapPixelFormat;
		BitmapPixelType bitmapPixelType = BitmapPixelType::Uint8;
		SelectPixelFormats(internalPixelFormat, bitmapPixelFormat, renderType);

		uint32 renderMode;
		SelectRenderMode(renderMode, renderType);

		bool isMonochrome = renderType == FontResolutionRenderType::Monochrome;		
		uint bitmapPixelDepth = GetFormatDepth(bitmapPixelFormat);

		uint i;

		i = 0;
		for (auto& metricsIT : characterMap)
		{
			uint character = metricsIT.first;
			uint glyphIndex = FT_Get_Char_Index(face, character);
			CharacterBitmap& bitmap = characterBitmaps[i];
			
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);

			//if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
			FT_Render_Glyph(face->glyph, (FT_Render_Mode)renderMode);

			if (metricsIT.second.size != Vec2f())
			{
				CopyBitmapData(bitmap, face->glyph->bitmap, isMonochrome);
				bitmap.size.x /= bitmapPixelDepth;
				bitmap.character = character;
								
				rectangles[i].x = 0;
				rectangles[i].y = 0;
				rectangles[i].w = characterBitmaps[i].size.x + 2;
				rectangles[i].h = characterBitmaps[i].size.y + 2;
			}
			else
			{
				rectangles[i].x = 0;
				rectangles[i].y = 0;
				rectangles[i].w = 0;
				rectangles[i].h = 0;
			}

			++i;
		}				

		CreateTexture(atlas, rectangles, internalPixelFormat);		

		Vec2i textureSize = atlas.GetSize();
		i = 0;
		for (auto& metricsIT : characterMap)
		{
			CharacterBitmap& bitmap = characterBitmaps[i];
			CharacterMetrics& metrics = metricsIT.second;

			if (bitmap.size.x != 0)
			{
				Vec2i offset = Vec2i(rectangles[i].x + 1, rectangles[i].y + 1);

				atlas.SetPixels(offset, bitmap.size, bitmap.stride, bitmapPixelFormat, bitmapPixelType, bitmap.data);
				
				metrics.uv1 = Vec2f(offset.x, offset.y) / Vec2f(textureSize);
				metrics.uv2 = Vec2f(offset.x + bitmap.size.x, offset.y + bitmap.size.y) / Vec2f(textureSize);				

				if (renderType == FontResolutionRenderType::SDF)
					metrics.renderOffset -= ((Vec2f)bitmap.size - metrics.size) / 2;
				metrics.size = (Vec2f)bitmap.size;
				std::swap(metrics.uv1.y, metrics.uv2.y);
			}
			else
			{
				metrics.uv1 = { };
				metrics.uv2 = { };
			}			

			++i;
		}	
		
		auto metr = characterMap['v'];

		for (uint i = 0; i < characterCount; ++i)
			FreeBitmapData(characterBitmaps[i]);
		delete[] characterBitmaps;				

		return Result();
	}

	CharacterMetrics FontResolution::GetCharacterData(UnicodeChar ch)
	{
		CharacterMetrics out;
		uint32 chValue = ch.Value();

		auto it = characterMap.find(chValue);
		if (it == characterMap.end())
		{
			out.advance = 0;
			out.renderOffset = { };
			out.size = { };
			out.uv1 = { };
			out.uv2 = { };
		}
		else
		{
			auto data = it->second;

			out.advance = data.advance;
			out.renderOffset = data.renderOffset;
			out.size = data.size;
			out.uv1 = data.uv1;
			out.uv2 = data.uv2;
			out.bitmapSize = data.bitmapSize;
		}
		return out;
	}

	Vec2f FontResolution::GetKerning(UnicodeChar left, UnicodeChar right)
	{
		FT_Face face = (FT_Face)font->ptr;
		uint l = FT_Get_Char_Index(face, left.Value());
		uint r = FT_Get_Char_Index(face, right.Value());
		FT_Vector vec;
		FT_Get_Kerning(face, l, r, FT_KERNING_UNSCALED, &vec);
		vec.x *= resolution / font->pixelsPerUnit;
		vec.y *= resolution / font->pixelsPerUnit;
		return Vec2f(vec.x, vec.y);
	}


	Font::Font()
		: ptr(nullptr), pixelsPerUnit(0), baselineDistance(0)
	{
	}
	Font::~Font()
	{
		for (auto& res : resolutions)
			delete res;
		if (ptr != nullptr)
		{			
			FT_Face face = (FT_Face)ptr;
			FT_Done_Face(face);
			ptr = nullptr;
		}		
	}

	Result Font::Load(StringView path)
	{
		FT_Error ftError;
		FT_Face face;

		//Load face
		{
			if (ptr != nullptr)
			{
				FT_Done_Face((FT_Face)ptr);
				ptr = nullptr;
			}

			ftError = FT_New_Face(GetFreeTypeLibrary(), path.Ptr(), 0, &face);

			if (ftError != 0)
				return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"BlazeEngine", "Failed to open font file!"));
			ptr = face;
		}

		if (!(face->face_flags & FT_FACE_FLAG_SCALABLE))
		{
			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"BlazeEngine", "Font file not supported, it's not a scalable font!"));			
		}

		FT_Select_Charmap(face, FT_ENCODING_UNICODE);
		bool kern = FT_HAS_KERNING(face);

		pixelsPerUnit = face->units_per_EM;
		baselineDistance = (float)face->height / pixelsPerUnit;

		return Result();
	}

	FontResolution* Font::CreateFontResolution(uint resolution, FontResolutionRenderType renderType)
	{
		FT_Error ftError;
		FT_Face face = (FT_Face)ptr;		
		
		FontResolution& res = *resolutions.emplace_back(new FontResolution);
		res.resolution = resolution;				
		res.renderType = renderType;
		res.baselineDistance = std::round(baselineDistance * resolution);
		res.font = this;				

		return &res;
	}

	inline FontResolution* Font::GetClosestResolution(float res) const
	{
		if (resolutions.size() == 0)
			return nullptr;

		FontResolution* upper = nullptr;
		FontResolution* lower = nullptr;

		for (auto& fontRes : resolutions)
		{
			int currRes = fontRes->GetResolution();

			if (currRes == res)
				return fontRes;
			else if (currRes < res && (lower == nullptr || currRes > lower->GetResolution()))
				lower = fontRes;
			else if (currRes > res && (upper == nullptr || currRes < upper->GetResolution()))
				upper = fontRes;
		}

		if (upper == nullptr)
			return lower;

		return upper;
	}
}