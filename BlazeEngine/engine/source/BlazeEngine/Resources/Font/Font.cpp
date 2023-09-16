#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/Console/Console.h"

#include "freetype/freetype.h"

#include "rectpack2D.h"

//#define MSDFGEN_PUBLIC
//#include "msdfgen/msdfgen.h"
//#include "msdfgen/msdfgen-ext.h"

namespace Blaze
{
	FT_Library GetFreeTypeLibrary();			

	namespace FontGlyphRenderers
	{		
		Bitmap MonochromeFontGlyphRenderer::Render(const Font& font, uint fontHeight, UnicodeChar character)
		{
			FT_Face face = (FT_Face)font.GetHandle();
			FT_Set_Pixel_Sizes(face, 0, fontHeight);
			uint glyphIndex = FT_Get_Char_Index(face, character.Value());
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			auto glyph = face->glyph;
			FT_Render_Glyph(glyph, FT_RENDER_MODE_MONO);

			Vec2i size = Vec2i(glyph->bitmap.width, glyph->bitmap.rows);

			Bitmap bitmap;
			bitmap.Create(size, BitmapColorFormat::Red, BitmapColorComponentType::Uint8, nullptr);

			void* dst = bitmap.GetPixels();
			uint dstStride = size.x;
			uint dstOffset = 0;

			void* src = glyph->bitmap.buffer;
			uint srcStride = glyph->bitmap.pitch;
			uint srcOffset = 0;

			for (uint y = 0; y < size.y; ++y)
			{
				for (uint x = 0; x < size.x; ++x)
					((uint8*)dst)[dstOffset + x] = ((((uint8*)src)[x / 8 + srcOffset] >> (7 - x % 8)) & 1) ? 255 : 0;

				dstOffset += dstStride;
				srcOffset += srcStride;
			}

			return bitmap;
		}
		
		Bitmap AntialiasedFontGlyphRenderer::Render(const Font& font, uint fontHeight, UnicodeChar character)
		{
			FT_Face face = (FT_Face)font.GetHandle();
			FT_Set_Pixel_Sizes(face, 0, fontHeight);
			uint glyphIndex = FT_Get_Char_Index(face, character.Value());
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			auto glyph = face->glyph;
			FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);

			Vec2i size = Vec2i(glyph->bitmap.width, glyph->bitmap.rows);

			Bitmap bitmap;
			bitmap.Create(size, BitmapColorFormat::Red, BitmapColorComponentType::Uint8, nullptr);

			void* dst = bitmap.GetPixels();
			uint dstStride = size.x;
			uint dstOffset = 0;

			void* src = glyph->bitmap.buffer;
			uint srcStride = glyph->bitmap.pitch;
			uint srcOffset = 0;

			for (uint y = 0; y < size.y; ++y)
			{
				memcpy((uint8*)dst + dstOffset, (uint8*)src + srcOffset, dstStride);

				dstOffset += dstStride;
				srcOffset += srcStride;
			}

			return bitmap;
		}
		
		Bitmap HorizontalLCDFontGlyphRenderer::Render(const Font& font, uint fontHeight, UnicodeChar character)
		{
			FT_Face face = (FT_Face)font.GetHandle();
			FT_Set_Pixel_Sizes(face, 0, fontHeight);
			uint glyphIndex = FT_Get_Char_Index(face, character.Value());
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			auto glyph = face->glyph;
			FT_Render_Glyph(glyph, FT_RENDER_MODE_LCD);

			Vec2i size = Vec2i(glyph->bitmap.width / 3, glyph->bitmap.rows);

			Bitmap bitmap;
			bitmap.Create(size, BitmapColorFormat::RGB, BitmapColorComponentType::Uint8, nullptr);

			void* dst = bitmap.GetPixels();
			uint dstStride = size.x * sizeof(Vec3<uint8>);
			uint dstOffset = 0;

			void* src = glyph->bitmap.buffer;
			uint srcStride = glyph->bitmap.pitch;
			uint srcOffset = 0;

			for (uint y = 0; y < size.y; ++y)
			{
				memcpy((uint8*)dst + dstOffset, (uint8*)src + srcOffset, dstStride);

				dstOffset += dstStride;
				srcOffset += srcStride;
			}

			return bitmap;
		}
		
		Bitmap VerticalLCDFontGlyphRenderer::Render(const Font& font, uint fontHeight, UnicodeChar character)
		{
			FT_Face face = (FT_Face)font.GetHandle();
			FT_Set_Pixel_Sizes(face, 0, fontHeight);
			uint glyphIndex = FT_Get_Char_Index(face, character.Value());
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			auto glyph = face->glyph;
			FT_Render_Glyph(glyph, FT_RENDER_MODE_LCD_V);

			Vec2i size = Vec2i(glyph->bitmap.width, glyph->bitmap.rows / 3);

			Bitmap bitmap;
			bitmap.Create(size, BitmapColorFormat::RGB, BitmapColorComponentType::Uint8, nullptr);

			void* dst = bitmap.GetPixels();
			uint dstStride = size.x * sizeof(Vec3<uint8>);
			uint dstOffset = 0;

			void* src = glyph->bitmap.buffer;
			uint srcStride = glyph->bitmap.pitch;
			uint srcOffset = 0;

			for (uint y = 0; y < size.y; ++y)
			{
				memcpy((uint8*)dst + dstOffset, (uint8*)src + srcOffset, dstStride);

				dstOffset += dstStride;
				srcOffset += srcStride;
			}

			return bitmap;
		}
		
		Bitmap SDFFontGlyphRenderer::Render(const Font& font, uint fontHeight, UnicodeChar character)
		{
			FT_Face face = (FT_Face)font.GetHandle();
			FT_Set_Pixel_Sizes(face, 0, fontHeight);
			uint glyphIndex = FT_Get_Char_Index(face, character.Value());
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			auto glyph = face->glyph;
			FT_Render_Glyph(glyph, FT_RENDER_MODE_SDF);

			Vec2i size = Vec2i(glyph->bitmap.width, glyph->bitmap.rows);

			Bitmap bitmap;
			bitmap.Create(size, BitmapColorFormat::Red, BitmapColorComponentType::Uint8, nullptr);

			void* dst = bitmap.GetPixels();
			uint dstStride = size.x;
			uint dstOffset = 0;

			void* src = glyph->bitmap.buffer;
			uint srcStride = glyph->bitmap.pitch;
			uint srcOffset = 0;

			for (uint y = 0; y < size.y; ++y)
			{
				memcpy((uint8*)dst + dstOffset, (uint8*)src + srcOffset, dstStride);

				dstOffset += dstStride;
				srcOffset += srcStride;
			}

			return bitmap;
		}

		/*
		msdfgen::Vector2 TranslateVector(FT_Vector value)
		{
			return msdfgen::Vector2((double)value.x / 64, (double)value.y / 64);
		}
		msdfgen::EdgeSegment* ResolvePoint(msdfgen::Vector2 point, msdfgen::Vector2* points, uint& pointCount, uint8 tag)
		{
			msdfgen::EdgeSegment* out = nullptr;
			if (tag == FT_CURVE_TAG_ON && pointCount != 0)
			{
				switch (pointCount)
				{
				case 1:
					out = new msdfgen::LinearSegment(points[0], point);
					break;
				case 2:
					out = new msdfgen::QuadraticSegment(points[0], points[1], point);
					break;
				case 3:
					out = new msdfgen::CubicSegment(points[0], points[1], points[2], point);
					break;
				default:
					break;
				}

				points[0] = point;
				pointCount = 1;
			}
			else if (tag == FT_CURVE_TAG_CONIC && pointCount == 2)
			{
				msdfgen::Vector2 midpoint;
				midpoint.x = (point.x + points[0].x) / 2;
				midpoint.y = (point.y + points[0].y) / 2;

				out = new msdfgen::QuadraticSegment(points[0], points[1], midpoint);

				points[0] = point;
				pointCount = 1;
			}
			else
			{
				points[pointCount] = point;
				++pointCount;
			}

			return out;
		}
		void CopyOutline(msdfgen::Shape& shape, FT_Outline outline)
		{
			uint contourBegin = 0;
			uint contourEnd = 0;

			for (uint i = 0; i < outline.n_contours; ++i)
			{
				contourEnd = outline.contours[i] + 1;

				auto& contour = shape.addContour();

				msdfgen::Vector2 firstPoint;
				uint8 firstTag;

				msdfgen::Vector2 points[3];
				uint pointCount = 0;

				firstPoint = TranslateVector(outline.points[contourBegin]);
				firstTag = FT_CURVE_TAG(outline.tags[contourEnd]);

				points[0] = firstPoint;
				pointCount = 1;

				for (uint j = contourBegin + 1; j < contourEnd; ++j)
				{
					msdfgen::Vector2 point = TranslateVector(outline.points[j]);
					uint8 tag = FT_CURVE_TAG(outline.tags[j]);

					auto edge = ResolvePoint(point, points, pointCount, tag);

					if (edge != nullptr)
						contour.addEdge(edge);
				}

				auto edge = ResolvePoint(firstPoint, points, pointCount, firstTag);

				if (edge != nullptr)
					contour.addEdge(edge);

				contourBegin = contourEnd;
			}
		}
		

		struct FontHandleRec
		{
			FT_Face face;
			bool ownership;
		};
		Bitmap MSDFFontGlyphRenderer::Render(const Font& font, uint fontHeight, UnicodeChar character)
		{
			FontHandleRec fontHandleRec;
			fontHandleRec.face = (FT_Face)font.GetHandle();
			fontHandleRec.ownership = false;

			msdfgen::FontHandle* fontHandle = (msdfgen::FontHandle*)&fontHandleRec;

			Vec2i size{ 32, 32 };
			msdfgen::Shape shape;	
			msdfgen::loadGlyph(shape, fontHandle, character.Value());

			shape.normalize();
			msdfgen::edgeColoringSimple(shape, 3.0);
			msdfgen::Bitmap<float, 4> tempBitmap(size.x, size.y);
			msdfgen::generateMTSDF(tempBitmap, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0));

			Bitmap bitmap;
			bitmap.Create(size, BitmapColorFormat::RGBA, BitmapColorComponentType::Uint8, nullptr);

			Vec4<uint8>* dstIt = (Vec4<uint8>*)bitmap.GetPixels();
			for (uint y = 0; y < size.y; ++y)
				for (uint x = 0; x < size.x; ++x)
				{
					*dstIt = Vec4<uint8>((*(Vec4f*)tempBitmap(x, y) + Vec4f((float)fontHeight / 8 / 2)) * (255.0f / fontHeight * 8));
					++dstIt;
				}

			msdfgen::Bitmap<float, 4> renderedBitmap { size.x, size.y};
			msdfgen::renderSDF(renderedBitmap, tempBitmap);
			String path = (String)"assets/glyphBitmaps/" + (char)character.Value() + "_msdfgen.png";
			msdfgen::saveTiff(tempBitmap, path.Ptr());
			//msdfgen::saveBmp
			path = (String)"assets/glyphBitmaps/" + (char)character.Value() + "_rendered.png";
			msdfgen::saveTiff(renderedBitmap, path.Ptr());
			bitmap.Save((String)"assets/glyphBitmaps/" + (char)character.Value() + ".bmp", BitmapSaveType::BMP);

			return bitmap;
		}
		*/
	}

	FontGlyphBitmaps::FontGlyphBitmaps()
	{
	}
	FontGlyphBitmaps::FontGlyphBitmaps(const Font& font, const FontResolution& fontResolution, FontGlyphRenderer& fontGlyphRenderer)
	{		
		glyphsData.Resize(fontResolution.GetNoneEmptyGlyphCount());

		uint index = 0;
		for (auto& glyphData : fontResolution.GetGlyphsData())
			if (glyphData.value.size.x != 0 && glyphData.value.size.y != 0)
			{
				glyphsData[index].character = glyphData.key;
				glyphsData[index].bitmap = fontGlyphRenderer.Render(font, fontResolution.GetFontHeight(), glyphData.key);
				++index;
			}
	}
	FontGlyphBitmaps::FontGlyphBitmaps(FontGlyphBitmaps&& other) noexcept
		: glyphsData(std::move(other.glyphsData))
	{

	}
	FontGlyphBitmaps::~FontGlyphBitmaps()
	{
		Clear();
	}
	void FontGlyphBitmaps::CreateAtlas(const CreateAtlasCallback& createAtlas, const CopyBitmapCallback& copyBitmap)
	{
			constexpr int glyphBitmapExtraPadding = 1;
						
		std::vector<rect_type> glyphAtlasRects { glyphsData.Count() };

		auto glyphAtlasRectIt = glyphAtlasRects.begin();
		for (auto& bitmap : glyphsData)
		{
			Vec2i size = bitmap.bitmap.GetSize() + Vec2i(glyphBitmapExtraPadding * 2);
			glyphAtlasRectIt->w = size.x;
			glyphAtlasRectIt->h = size.y;
			++glyphAtlasRectIt;
		}

		const auto result_size = PackRects(glyphAtlasRects);

		createAtlas(Vec2i(result_size.w, result_size.h));

		for (uint i = 0; i < glyphsData.Count(); ++i)
		{
			Vec2i offset = Vec2i(glyphAtlasRects[i].x, glyphAtlasRects[i].y) + Vec2i(glyphBitmapExtraPadding);

			copyBitmap(glyphsData[i].character, offset, glyphsData[i].bitmap);
		}		
	}
	void FontGlyphBitmaps::Clear()
	{
		glyphsData.Clear();
	}
	FontGlyphBitmaps& FontGlyphBitmaps::operator=(FontGlyphBitmaps&& other) noexcept
	{
		glyphsData = std::move(other.glyphsData);

		return *this;
	}

	FontResolution::FontResolution()
		: fontHeight(0), baselineDistance(0), nonEmptyCount(0)
	{
	}
	FontResolution::FontResolution(const Font& font, uint fontHeight, const CharacterSet& characterSet)
		: fontHeight(fontHeight), baselineDistance(0), nonEmptyCount(0)
	{
		FT_Face face = (FT_Face)font.GetHandle();

		FT_Set_Pixel_Sizes(face, 0, fontHeight);
				
		baselineDistance = std::round(baselineDistance * fontHeight);		
		nonEmptyCount = 0;

		for (auto& span : characterSet.spans)
			for (UnicodeChar character = span.first; character != span.last; ++character)
			{
				uint glyphIndex = FT_Get_Char_Index(face, character.Value());

				if (glyphIndex == 0)
					continue;

				FontResolutionGlyphData data;

				FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
				FT_GlyphSlot glyph = face->glyph;

				data.size = Vec2f(glyph->metrics.width, glyph->metrics.height) / 64.0f;
				data.horizontalAdvance = float(glyph->metrics.horiAdvance) / 64.0f;
				data.verticalAdvance = float(glyph->metrics.vertAdvance) / 64.0f;
				data.offset = Vec2f(glyph->metrics.horiBearingX, glyph->metrics.horiBearingY - glyph->metrics.height) / 64.0f;

				if (data.size.x != 0 && data.size.y != 0)
					++nonEmptyCount;

				glyphsData.Insert(character, data);
			}
	}
	FontResolution::FontResolution(FontResolution&& other) noexcept
		: glyphsData(std::move(other.glyphsData))
	{
		fontHeight = other.fontHeight;
		baselineDistance = other.baselineDistance;
		nonEmptyCount = other.nonEmptyCount;

		other.fontHeight = 0;
		other.baselineDistance = 0;
		other.nonEmptyCount = 0;
	}
	FontResolution::~FontResolution()
	{
		Clear();
	}
	bool FontResolution::GetGlyphData(UnicodeChar character, FontResolutionGlyphData& metrics) const
	{
		auto it = glyphsData.Find(character);

		if (it.IsNull())
			return false;

		metrics = it->value;
		return true;
	}		
	void FontResolution::Clear()
	{
		fontHeight = 0;
		baselineDistance = 0;
		nonEmptyCount = 0;
		glyphsData.Clear();
	}

	CharacterSet::CharacterSet()
	{

	}
	CharacterSet::CharacterSet(CharacterSet&& other) noexcept
		: spans(std::move(other.spans))
	{

	}
	CharacterSet::CharacterSet(Array<CharacterSpan> spans)
		: spans(std::move(spans))
	{

	}
	CharacterSet CharacterSet::ASCIICharacterSet()
	{
		CharacterSet cs;
		cs.spans.AddBack(CharacterSpan({ .first = 0, .last = 128 }));
		return cs;
	}
	CharacterSet& CharacterSet::operator=(CharacterSet&& other) noexcept
	{
		spans = std::move(other.spans);
		return *this;
	}

	Font::Font()
		: memory(nullptr), ptr(nullptr), pixelsPerUnit(1), baselineDistance(0)
	{
	}
	Font::Font(Path path)
		: ptr(nullptr), pixelsPerUnit(1), baselineDistance(0)
	{
		Load(std::move(path));
	}
	Font::~Font()
	{		
		Clear();
	}
	Result Font::Load(Path path)
	{		
		File file;		
		
		CHECK_RESULT(file.Open(path, FileAccessPermission::Read));

		return Load(file);
	}
	Result Font::Load(ReadStream& readStream)
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

			uint64 memorySize = readStream.GetSize();
			memory = Memory::Allocate(memorySize);
			readStream.Read(memory, memorySize);

			ftError = FT_New_Memory_Face(GetFreeTypeLibrary(), (const FT_Byte*)memory, memorySize, 0, &(FT_Face&)ptr);				

			if (ftError != 0)
			{
				Memory::Free(memory);
				memory = nullptr;
				ptr = nullptr;

				return BLAZE_WARNING_RESULT("BlazeEngine", "Failed to open font file!");
			}
		}

		FT_Face face = (FT_Face)ptr;

		if (!(face->face_flags & FT_FACE_FLAG_SCALABLE))
		{
			return BLAZE_WARNING_RESULT("BlazeEngine", "Font file not supported, it's not a scalable font!");
		}

		FT_Select_Charmap(face, FT_ENCODING_UNICODE);
		bool kern = FT_HAS_KERNING(face);

		pixelsPerUnit = face->units_per_EM;
		baselineDistance = (float)face->height / pixelsPerUnit;

		return Result();
	}
	void Font::Clear()
	{		
		resolutions.Clear();

		if (ptr != nullptr)
		{
			FT_Done_Face((FT_Face)ptr);
			ptr = nullptr;
		}

		if (memory != nullptr)
		{
			Memory::Free(memory);
		}
	}
	Vec2f Font::GetKerning(const FontResolution& fontResolution, UnicodeChar left, UnicodeChar right) const
	{	
		FT_Face face = (FT_Face)ptr;
		uint l = FT_Get_Char_Index(face, left.Value());
		uint r = FT_Get_Char_Index(face, right.Value());
		FT_Vector vec;
		FT_Get_Kerning(face, l, r, FT_KERNING_UNSCALED, &vec);
		vec.x *= fontResolution.GetFontHeight() / pixelsPerUnit;
		vec.y *= fontResolution.GetFontHeight() / pixelsPerUnit;
		return Vec2f(vec.x, vec.y);
	}
}