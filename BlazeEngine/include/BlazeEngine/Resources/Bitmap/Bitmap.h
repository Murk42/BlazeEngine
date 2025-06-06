#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/File/Path.h"
#include "BlazeEngine/BlazeEngineDefines.h"

namespace Blaze
{			
	class WriteStream;
	class ReadStream;

	enum class BitmapColorFormat 
	{
		Red,
		RG,
		RGB,
		RGBA,
		BGR,
		BGRA	
	};
	BLAZE_API uint BitmapColorFormatComponentCount(BitmapColorFormat format);

	enum class BitmapColorComponentType
	{
		Int8,
		Uint8,
		Int16,
		Uint16,
		Int32,
		Uint32,
		Float,
		Double
	};
	BLAZE_API uintMem BitmapColorComponentTypeSize(BitmapColorComponentType type);	

	class BitmapView;

	class BLAZE_API BitmapRef
	{
	public:
		BitmapRef();
		BitmapRef(Vec2u size, BitmapColorFormat format, BitmapColorComponentType type, uintMem stride, void* pixels);
		BitmapRef(const BitmapRef& other);

		inline bool Empty() const { return pixels == nullptr; }
		inline uintMem GetStride() const { return stride; }
		inline void* GetPixels() const { return pixels; }
		inline Vec2u GetSize() const { return size; }
		inline uintMem GetPixelSize() const { return BitmapColorFormatComponentCount(format) * BitmapColorComponentTypeSize(type); }
		inline uintMem GetByteWidth() const { return GetPixelSize() * size.x; }
		inline uintMem GetPadding() const { return stride - GetByteWidth(); }
		inline BitmapColorFormat GetPixelFormat() const { return format; }
		inline BitmapColorComponentType GetPixelType() const { return type; }

		BitmapRef GetSubBitmap(Vec2u size, Vec2u offset);
		
		BitmapRef& operator=(const BitmapRef& other);
	protected:
		void* pixels;
		uintMem stride;
		Vec2u size;
		BitmapColorFormat format;
		BitmapColorComponentType type;
	};
		
	class BLAZE_API Bitmap : public BitmapRef
	{		
	public:
		Bitmap();
		Bitmap(const Bitmap&);
		Bitmap(Bitmap&&) noexcept;
		Bitmap(const BitmapView& bitmapView, uintMem newStride);
		~Bitmap();				

		bool Load(Path path, bool flipVertically = false);
		bool Load(ReadStream& stream, bool flipVertically = false);
		bool LoadWithMIME(ReadStream& stream, StringUTF8 imageMIMEType, bool flipVertically = false);
		bool LoadWithExtension(ReadStream& stream, StringUTF8 extension, bool flipVertically = false);
		bool Save(Path path);
		bool Save(WriteStream& stream, StringUTF8 imageType);
				
		void Create(Vec2u size, BitmapColorFormat format, BitmapColorComponentType type, const void* pixels, uintMem stride = 0, bool flipVertically = false);

		void Clear();		

		Bitmap& operator=(const Bitmap&);
		Bitmap& operator=(Bitmap&&) noexcept;
	};

	class BLAZE_API BitmapView
	{
	public:
		BitmapView();
		BitmapView(Vec2u size, BitmapColorFormat format, BitmapColorComponentType type, uintMem stride, const void* pixels);
		BitmapView(const Bitmap& other);
		BitmapView(const BitmapRef& other);
		BitmapView(const BitmapView& other);

		inline uintMem GetStride() const { return stride; }
		inline const void* GetPixels() const { return pixels; }
		inline Vec2u GetSize() const { return size; }
		inline uintMem GetPixelSize() const { return BitmapColorFormatComponentCount(format) * BitmapColorComponentTypeSize(type); }
		inline uintMem GetByteWidth() const { return GetPixelSize() * size.x; }
		inline uintMem GetPadding() const { return stride - GetByteWidth(); }
		inline BitmapColorFormat GetPixelFormat() const { return format; }
		inline BitmapColorComponentType GetPixelType() const { return type; }		

		BitmapView GetSubBitmap(Vec2u size, Vec2u offset);

		BitmapView& operator=(const Bitmap& other);
		BitmapView& operator=(const BitmapRef& other);
		BitmapView& operator=(const BitmapView& other);
	private:
		const void* pixels;
		uintMem stride;
		Vec2u size;
		BitmapColorFormat format;
		BitmapColorComponentType type;
	};	
}

