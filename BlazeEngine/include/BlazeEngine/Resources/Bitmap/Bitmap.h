#pragma once
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/File/Stream/BufferStream.h"

namespace Blaze
{			
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
	BLAZE_API uint BitmapColorComponentTypeSize(BitmapColorComponentType type);

	enum class BitmapSaveType
	{
		BMP = 0x0420,
		CHEAD = 0x042F,
		JPG = 0x0425,
		PNM = 0x042B,
		PSD = 0x0439,
		SGI = 0x042C,
		TGA = 0x042D,
	};

	class BLAZE_API BitmapRef
	{
	public:
		BitmapRef();
		BitmapRef(Vec2i size, BitmapColorFormat format, BitmapColorComponentType type, uint stride, void* pixels);		
		BitmapRef(const BitmapRef& other);

		inline uint GetStride() const { return stride; }
		inline void* GetPixels() const { return pixels; }
		inline Vec2i GetSize() const { return size; }
		inline BitmapColorFormat GetPixelFormat() const { return format; }
		inline BitmapColorComponentType GetPixelType() const { return type; }
		
		BitmapRef& operator=(const BitmapRef& other);
	protected:
		void* pixels;
		uint stride;
		Vec2i size;
		BitmapColorFormat format;
		BitmapColorComponentType type;
	};
		
	class BLAZE_API Bitmap : public BitmapRef
	{		
	public:
		Bitmap();
		Bitmap(const Bitmap&);
		Bitmap(Bitmap&&) noexcept;
		~Bitmap();		

		Result Load(Path path, bool flipVertically = false);		
		Result Save(Path path);
		Result Save(Path path, BitmapSaveType type);		

		Result Serialize(WriteStream& stream) const;
		Result Deserialize(ReadStream& stream);
		
		Result Create(Vec2i size, BitmapColorFormat format, BitmapColorComponentType type, const void* pixels, bool flipVertically = false);			 

		void Clear();		

		Bitmap& operator=(const Bitmap&);
		Bitmap& operator=(Bitmap&&) noexcept;
	};

	class BLAZE_API BitmapView
	{
	public:
		BitmapView();
		BitmapView(Vec2i size, BitmapColorFormat format, BitmapColorComponentType type, uint stride, const void* pixels);
		BitmapView(const Bitmap& other);
		BitmapView(const BitmapRef& other);
		BitmapView(const BitmapView& other);

		inline uint GetStride() const { return stride; }
		inline const void* GetPixels() const { return pixels; }
		inline Vec2i GetSize() const { return size; }
		inline BitmapColorFormat GetPixelFormat() const { return format; }
		inline BitmapColorComponentType GetPixelType() const { return type; }

		BitmapView& operator=(const Bitmap& other);
		BitmapView& operator=(const BitmapRef& other);
		BitmapView& operator=(const BitmapView& other);
	private:
		const void* pixels;
		uint stride;
		Vec2i size;
		BitmapColorFormat format;
		BitmapColorComponentType type;
	};	
}

