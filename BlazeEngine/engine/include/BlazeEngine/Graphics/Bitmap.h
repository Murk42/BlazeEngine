#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/Vector.h"

namespace Blaze
{		
	enum class PixelFormat {
		Red = 0x1903,
		RG = 0x8227,
		RGB = 0x1907,
		RGBA = 0x1908,
		BGR = 0x80E0,
		BGRA = 0x80E1,
	};

	uint BLAZE_API GetFormatDepth(PixelFormat format);	

	class BLAZE_API Bitmap
	{
		PixelFormat format;
		Vec2i size;
		void* pixels;

		unsigned id;

		Bitmap(const Bitmap&) = delete;
		void operator=(const Bitmap&) = delete;
	public:
		Bitmap();
		~Bitmap();

		bool Load(const String& path, bool emitWarning = true);
		bool Save(const String& path, bool emitWarning = true);

		void Create(const Bitmap& bm);
		void Create(const Vec2i& size, PixelFormat format, void* pixels = nullptr);			

		void SetPixels(Vec2i offset, Vec2i size, PixelFormat format, void* pixels);

		void ConvertFormat(PixelFormat format);
		void FlipVerticaly();

		PixelFormat GetFormat() const { return format; }
		Vec2i GetSize() const { return size; }
		void* GetPixels() const { return pixels; }		

		friend class Texture2D;
	};
}

