#include "pch.h"
#include "BlazeEngine/Resources/Font/Font.h"

#include "freetype/freetype.h"

#include "rectpack2D.h"

#include "BlazeEngine/Files/Consola.h"

namespace Blaze
{
	FT_Library GetFreeTypeLibrary();

	CharacterSetIterator::CharacterSetIterator()
		: characterSet(nullptr), spanIndex(0), characterIndex(0)
	{
	}
	CharacterSetIterator::CharacterSetIterator(const CharacterSetIterator& other)
		: characterSet(other.characterSet), spanIndex(other.spanIndex), characterIndex(other.characterIndex)
	{
	}
	CharacterSetIterator::~CharacterSetIterator()
	{
	}
	bool CharacterSetIterator::IsNull() const
	{
		return characterSet == nullptr;
	}
	CharacterSetIterator& CharacterSetIterator::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (characterSet == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Incrementing a null iterator");
#endif

		++characterIndex;

		auto span = characterSet->spans[spanIndex];

		if (characterIndex == span.last.Value() - span.first.Value())
		{
			++spanIndex;
			characterIndex = 0;

			if (spanIndex == characterSet->spans.Count())
			{
				spanIndex = 0;
				characterSet = nullptr;
			}
		}

		return *this;
	}
	CharacterSetIterator CharacterSetIterator::operator++(int)
	{
		CharacterSetIterator copy;
		++copy;
		return copy;
	}
	CharacterSetIterator& CharacterSetIterator::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (characterSet == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Decrementing a null iterator");
#endif


		if (characterIndex == 0)
			if (spanIndex == 0)
			{
				spanIndex = 0;
				characterSet = nullptr;
			}
			else
			{
				--spanIndex;
				auto span = characterSet->spans[spanIndex];
				characterIndex = span.last.Value() - span.first.Value();
			}
		else
			--characterIndex;

		return *this;
	}
	CharacterSetIterator CharacterSetIterator::operator--(int)
	{
		CharacterSetIterator copy = *this;
		--copy;
		return copy;
	}
	UnicodeChar CharacterSetIterator::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (characterSet == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
#endif
		return UnicodeChar(characterSet->spans[spanIndex].first.Value() + characterIndex);
	}
	bool CharacterSetIterator::operator==(CharacterSetIterator& other) const
	{
		return characterSet == other.characterSet && spanIndex == other.spanIndex && characterIndex == other.characterIndex;
	}
	bool CharacterSetIterator::operator!=(CharacterSetIterator& other) const
	{
		return characterSet != other.characterSet || spanIndex != other.spanIndex || characterIndex != other.characterIndex;
	}
	CharacterSetIterator& CharacterSetIterator::operator=(const CharacterSetIterator& other)
	{
		characterSet = other.characterSet;
		spanIndex = other.spanIndex;
		characterIndex = other.characterIndex;

		return *this;
	}
	CharacterSetIterator::CharacterSetIterator(const CharacterSet* characterSet, uintMem spanIndex, uintMem characterIndex)
		: characterSet(characterSet), spanIndex(spanIndex), characterIndex(characterIndex)
	{
	}

	CharacterSet::CharacterSet()
		: count(0)
	{

	}
	CharacterSet::CharacterSet(CharacterSet&& other) noexcept
		: spans(std::move(other.spans)), count(other.count)
	{
		other.count = 0;
	}
	CharacterSet::CharacterSet(Array<CharacterSpan> spans)
		: spans(std::move(spans)), count(0)
	{
		for (auto& span : spans)
			count += span.last.Value() - span.first.Value();
	}
	CharacterSet CharacterSet::ASCIICharacterSet()
	{
		CharacterSet cs;
		cs.spans.AddBack(CharacterSpan({ .first = 0, .last = 127 }));
		cs.count = 128;
		return cs;
	}
	uintMem CharacterSet::Count() const
	{
		return count;
	}
	CharacterSetIterator CharacterSet::FirstIterator() const
	{
		return CharacterSetIterator(this, 0, 0);
	}
	CharacterSetIterator CharacterSet::BehindIterator() const
	{
		return CharacterSetIterator(nullptr, 0, 0);
	}
	CharacterSet& CharacterSet::operator=(CharacterSet&& other) noexcept
	{
		spans = std::move(other.spans);
		count = other.count;

		other.count = 0;
		return *this;
	}

	CharacterSetIterator begin(const CharacterSet& characterSet)
	{
		return characterSet.FirstIterator();
	}
	CharacterSetIterator end(const CharacterSet& characterSet)
	{
		return characterSet.BehindIterator();
	}

	namespace FontGlyphRenderers
	{
		Bitmap MonochromeFontGlyphRenderer::Render(const Font& font, uint pixelFontHeight, UnicodeChar character)
		{
			FT_Face face = (FT_Face)font.GetHandle();
			FT_Set_Pixel_Sizes(face, 0, pixelFontHeight);
			uint glyphIndex = FT_Get_Char_Index(face, character.Value());
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			auto glyph = face->glyph;
			FT_Render_Glyph(glyph, FT_RENDER_MODE_MONO);

			Vec2u size = Vec2u(glyph->bitmap.width, glyph->bitmap.rows);

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

		Bitmap AntialiasedFontGlyphRenderer::Render(const Font& font, uint pixelFontHeight, UnicodeChar character)
		{
			FT_Face face = (FT_Face)font.GetHandle();
			FT_Set_Pixel_Sizes(face, 0, pixelFontHeight);
			uint glyphIndex = FT_Get_Char_Index(face, character.Value());
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			auto glyph = face->glyph;
			FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);

			Vec2u size = Vec2u(glyph->bitmap.width, glyph->bitmap.rows);

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

		Bitmap HorizontalLCDFontGlyphRenderer::Render(const Font& font, uint pixelFontHeight, UnicodeChar character)
		{
			FT_Face face = (FT_Face)font.GetHandle();
			FT_Set_Pixel_Sizes(face, 0, pixelFontHeight);
			uint glyphIndex = FT_Get_Char_Index(face, character.Value());
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			auto glyph = face->glyph;
			FT_Render_Glyph(glyph, FT_RENDER_MODE_LCD);

			Vec2u size = Vec2u(glyph->bitmap.width / 3, glyph->bitmap.rows);

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

		Bitmap VerticalLCDFontGlyphRenderer::Render(const Font& font, uint pixelFontHeight, UnicodeChar character)
		{
			FT_Face face = (FT_Face)font.GetHandle();
			FT_Set_Pixel_Sizes(face, 0, pixelFontHeight);
			uint glyphIndex = FT_Get_Char_Index(face, character.Value());
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			auto glyph = face->glyph;
			FT_Render_Glyph(glyph, FT_RENDER_MODE_LCD_V);

			Vec2u size = Vec2u(glyph->bitmap.width, glyph->bitmap.rows / 3);

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

		Bitmap SDFFontGlyphRenderer::Render(const Font& font, uint pixelFontHeight, UnicodeChar character)
		{
			FT_Face face = (FT_Face)font.GetHandle();
			FT_Set_Pixel_Sizes(face, 0, pixelFontHeight);
			uint glyphIndex = FT_Get_Char_Index(face, character.Value());
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			auto glyph = face->glyph;
			FT_Render_Glyph(glyph, FT_RENDER_MODE_SDF);

			Vec2u size = Vec2u(glyph->bitmap.width, glyph->bitmap.rows);

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
		Bitmap MSDFFontGlyphRenderer::Render(const Font& font, uint pixelFontHeight, UnicodeChar character)
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
					*dstIt = Vec4<uint8>((*(Vec4f*)tempBitmap(x, y) + Vec4f((float)pixelFontHeight / 8 / 2)) * (255.0f / pixelFontHeight * 8));
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

	Vec2f FontMetrics::GetGlyphKerning(UnicodeChar left, UnicodeChar right) const
	{
		FT_Face face = (FT_Face)font->ptr;

		if (pixelFontHeight != 0)
			FT_Set_Pixel_Sizes(face, pixelFontHeight, pixelFontHeight);

		uint l = FT_Get_Char_Index(face, left.Value());
		uint r = FT_Get_Char_Index(face, right.Value());
		FT_Vector vec;

		if (pixelFontHeight != 0)
		{
			FT_Get_Kerning(face, l, r, FT_KERNING_DEFAULT, &vec);
			return Vec2f(vec.x, vec.y) / 64.0f;
		}
		else
		{
			FT_Get_Kerning(face, l, r, FT_KERNING_UNSCALED, &vec);
			return Vec2f(vec.x, vec.y) / 64.0f / font->GetFontUnitScale();
		}
	}
	bool FontMetrics::GetGlyphMetrics(UnicodeChar character, FontGlyphMetrics& data) const
	{
		FT_Face face = (FT_Face)font->ptr;

		if (face == nullptr)
		{
			Debug::Logger::LogWarning("Blaze Engine", "Trying to get a glyph metrics from a font object that wasn't loaded from anywhere");
			return false;
		}

		auto it = glyphMetrics.Find(character);
		if (!it.IsNull())
		{
			data = it->value;
			return true;
		}

		if (pixelFontHeight != 0)
			FT_Set_Pixel_Sizes(face, pixelFontHeight, pixelFontHeight);

		uint glyphIndex = FT_Get_Char_Index(face, character.Value());

		if (glyphIndex == 0)
			return false;

		float scale = 0.0f;
		if (pixelFontHeight != 0)
		{
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_NO_BITMAP | FT_LOAD_BITMAP_METRICS_ONLY);
			scale = 1.0f / 64.0f;
		}
		else
		{
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_NO_SCALE);
			scale = 1.0f / font->GetFontUnitScale();
		}

		FT_GlyphSlot glyph = face->glyph;

		data.size = Vec2f(glyph->metrics.width, glyph->metrics.height) * scale;
		data.horizontalAdvance = glyph->metrics.horiAdvance * scale;
		data.verticalAdvance = glyph->metrics.vertAdvance * scale;
		data.offset = Vec2f(glyph->metrics.horiBearingX, glyph->metrics.horiBearingY - glyph->metrics.height) * scale;

		glyphMetrics.Insert(character, data);

		return true;
	}

	Map<UnicodeChar, Bitmap> FontMetrics::GetGlyphBitmaps(const CharacterSet& characterSet, FontGlyphRenderer& glyphRenderer) const
	{
		FT_Face face = (FT_Face)font->ptr;
		if (face == nullptr)
		{
			Debug::Logger::LogWarning("Blaze Engine", "Trying to get a glyph bitmaps from a font object that wasn't loaded from anywhere");
			return { };
		}

		Map<UnicodeChar, Bitmap> bitmaps;

		for (auto character : characterSet)
			bitmaps.Insert(character, glyphRenderer.Render(*font, pixelFontHeight, character));

		return bitmaps;
	}
	void FontMetrics::CreateAtlas(const CharacterSet& characterSet, FontGlyphRenderer& glyphRenderer, const CreateAtlasCallback& createAtlasCallback, const CopyBitmapCallback& copyBitmapCallback) const
	{
		FT_Face face = (FT_Face)font->ptr;
		if (face == nullptr)
		{
			Debug::Logger::LogWarning("Blaze Engine", "Trying to create a atlas from a font object that wasn't loaded from anywhere");
			return;
		}

		constexpr int glyphBitmapExtraPadding = 1;

		std::vector<rect_type> glyphAtlasRects{ characterSet.Count() };

		Array<Bitmap> bitmaps;
		bitmaps.ReserveExactly(characterSet.Count());

		auto glyphAtlasRectIt = glyphAtlasRects.begin();
		for (auto character : characterSet)
		{
			Bitmap& bitmap = *bitmaps.AddBack(glyphRenderer.Render(*font, pixelFontHeight, character));
			Vec2u size = bitmap.GetSize() + Vec2u(glyphBitmapExtraPadding * 2);
			glyphAtlasRectIt->w = (int)size.x;
			glyphAtlasRectIt->h = (int)size.y;
			++glyphAtlasRectIt;
		}

		const auto result_size = PackRects(glyphAtlasRects);

		createAtlasCallback(Vec2u(result_size.w, result_size.h));

		uintMem i = 0;
		for (auto character : characterSet)
		{
			Bitmap& bitmap = bitmaps[i];
			Vec2u offset = Vec2u(glyphAtlasRects[i].x, glyphAtlasRects[i].y) + Vec2u(glyphBitmapExtraPadding);

			copyBitmapCallback(character, offset, bitmap);

			++i;
		}
	}
	FontMetrics::FontMetrics(Font* font, uint pixelFontHeight)
		: font(font), pixelFontHeight(pixelFontHeight)
	{
	}

	Font::Font()
		: memory(nullptr), ptr(nullptr)
	{
	}
	Font::Font(Font&& other) noexcept
		: ptr(other.ptr), memory(other.memory), metrics(std::move(other.metrics)), dataMap(std::move(other.dataMap))
	{
		for (auto& metric : metrics)
			metric.value.font = this;

		other.ptr = nullptr;
		other.memory = nullptr;
	}
	Font::Font(Path path)
		: ptr(nullptr), memory(nullptr)
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

			uintMem memorySize = readStream.GetSize();
			memory = Memory::Allocate(memorySize);
			readStream.Read(memory, memorySize);

			ftError = FT_New_Memory_Face(GetFreeTypeLibrary(), (const FT_Byte*)memory, (FT_Long)memorySize, 0, &(FT_Face&)ptr);

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

		return Result();
	}
	FontMetrics& Font::GetUnscalledMetrics() const
	{
		return metrics.Insert(0, (Font*)this, 0).iterator->value;
	}
	FontMetrics& Font::GetMetrics(uint pixelFontHeight) const
	{
		return metrics.Insert(pixelFontHeight, (Font*)this, pixelFontHeight).iterator->value;
	}
	uint Font::GetFontUnitScale() const
	{
		if (ptr == nullptr)
			return 0;
		return ((FT_Face)ptr)->units_per_EM;
	}
	void Font::Clear()
	{
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
	Font& Font::operator=(Font&& other) noexcept
	{
		ptr = other.ptr;
		memory = other.memory;
		metrics = std::move(other.metrics);
		dataMap = std::move(other.dataMap);

		for (auto& metric : metrics)
			metric.value.font = this;

		other.ptr = nullptr;
		other.memory = nullptr;
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