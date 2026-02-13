#include "pch.h"
#include "BlazeEngine/UI/Text/FontGlyphRasterizer.h"
#include "BlazeEngine/UI/Text/FontFace.h"
#include <freetype/freetype.h>

namespace Blaze::UI::FontGlyphRasterizers
{
	Bitmap MonochromeFontGlyphRasterizer::Render(const FontFace& fontFace, float rasterFontHeight, uint32 glyphIndex) const
	{
		FT_Face face = (FT_Face)fontFace.GetHandle();

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

	Bitmap AntialiasedFontGlyphRasterizer::Render(const FontFace& fontFace, float rasterFontHeight, uint32 glyphIndex) const
	{
		FT_Face face = (FT_Face)fontFace.GetHandle();
		FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(rasterFontHeight));

		FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
		auto glyph = face->glyph;
		FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);

		Vec2u size = Vec2u(glyph->bitmap.width, glyph->bitmap.rows);

		if (size.x == 0 || size.y == 0)
			return Bitmap();

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

	Bitmap HorizontalLCDFontGlyphRasterizer::Render(const FontFace& fontFace, float rasterFontHeight, uint32 glyphIndex) const
	{
		FT_Face face = (FT_Face)fontFace.GetHandle();
		FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(rasterFontHeight));

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

	Bitmap VerticalLCDFontGlyphRasterizer::Render(const FontFace& fontFace, float rasterFontHeight, uint32 glyphIndex) const
	{
		FT_Face face = (FT_Face)fontFace.GetHandle();
		FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(rasterFontHeight));

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

	Bitmap SDFFontGlyphRasterizer::Render(const FontFace& fontFace, float rasterFontHeight, uint32 glyphIndex) const
	{
		FT_Face face = (FT_Face)fontFace.GetHandle();
		FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(rasterFontHeight));

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
	Bitmap MSDFFontGlyphRasterizer::Render(const FontFace& fontFace, uint32 glyphIndex)
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
				*dstIt = Vec4<uint8>((*(Vec4f*)tempBitmap(x, y) + Vec4f((float)fontPixelHeight / 8 / 2)) * (255.0f / fontPixelHeight * 8));
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