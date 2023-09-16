#include "BlazeEngine/Resources/Bitmap/Bitmap.h"


namespace Blaze
{
	BitmapRef::BitmapRef()
		: pixels(nullptr), format(BitmapColorFormat::RGBA), type(BitmapColorComponentType::Int32), size(0, 0), stride(0)
	{

	}
	BitmapRef::BitmapRef(const BitmapRef& other)
	{

	}

	Bitmap::Bitmap()
		: BitmapRef()
	{
	}
	Bitmap::Bitmap(const Bitmap& other)		
	{
		uint componentCount = BitmapColorFormatComponentCount(other.format);
		uint componentSize = BitmapColorComponentTypeSize(other.type);
		uint pixelSize = componentCount * componentSize;
		uint bitmapSize = other.stride * other.size.y;

		pixels = Memory::Allocate(bitmapSize);
		memcpy(pixels, other.pixels, bitmapSize);

		size = other.size;
		type = other.type;
		format = other.format;
		stride = other.stride;
	}
	Bitmap::Bitmap(Bitmap&& other) noexcept
		: BitmapRef(other)
	{
		other.pixels = nullptr;
		other.size = Vec2i();
		other.stride = 0;
	}
	Bitmap::~Bitmap()
	{
		Clear();
	}	

	void Bitmap::Clear()
	{
		Memory::Free(pixels);
		pixels = nullptr;
		size = Vec2i();
		stride = 0;
	}

	Result Bitmap::Load(Path path, bool flipVertically)
	{
		Clear();

		File file{ path, FileAccessPermission::Read };

		//ImageGuard imageGuard;				
		//		
		//if (!ilLoadF(IL_TYPE_UNKNOWN, (FileWriteStream*)&file))
		//	return BLAZE_ERROR_RESULT("DevIL", "Failed to load image with a file read stream");		
		//
		//void* pixels = ilGetData();
		//
		//auto format = DevILToBlazePixelFormat(ilGetInteger(IL_IMAGE_FORMAT));
		//auto type = DevILToBlazePixelType(ilGetInteger(IL_IMAGE_TYPE));		
		//auto size = Vec2i(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

		Create(size, format, type, pixels, flipVertically);

		return Result();
	}	
	Result Bitmap::Save(Path path)
	{		
		File file{ path, FileAccessPermission::Write };

		//ImageGuard imageGuard;
		//
		//Result result;		
		//
		//auto componentCount = BitmapColorFormatComponentCount(format);
		//auto colorFormat = DevILColorFormat(format);
		//auto colorComponentType = DevILColorComponentType(type);
		//auto colorComponentTypeSize = BitmapColorComponentTypeSize(type);
		//CHECK_RESULT(result);
		//
		//if (stride == 0)
		//{
		//	if (!ilTexImage(size.x, size.y, 1, componentCount, colorFormat, colorComponentType, pixels))
		//		return BLAZE_ERROR_RESULT("DevIL", "Failed to create DevIL image. IL error: \"" + GetILErrorString() + "\"");			
		//}
		//else
		//{
		//
		//}
		//
		//if (!ilSaveImage(path.GetString().Ptr()))
		//	return BLAZE_ERROR_RESULT("DevIL", "Failed to save image. With IL error: \"" + GetILErrorString() + "\"");

		return Result();
	}
	Result Bitmap::Save(Path path, BitmapSaveType saveType)
	{
		File file{ path, FileAccessPermission::Write };

		//if (!file.IsOpen())
		//	return BLAZE_ERROR_RESULT("Blaze Engine", "File isnt opened");
		//
		//ImageGuard imageGuard;
		//
		//Result result;
		//
		//auto componentCount = BitmapColorFormatComponentCount(format);
		//auto colorFormat = DevILColorFormat(format);
		//auto colorComponentType = DevILColorComponentType(type);
		//auto colorComponentTypeSize = BitmapColorComponentTypeSize(type);
		//CHECK_RESULT(result);
		//
		//if (!ilTexImage(size.x, size.y, 1, componentCount, colorFormat, colorComponentType, pixels))
		//	return BLAZE_ERROR_RESULT("DevIL", "Failed to create DevIL image. IL error: \"" + GetILErrorString() + "\"");
		//
		//if (!ilSaveF((ILenum)saveType, (FileWriteStream*)&file))
		//	return BLAZE_ERROR_RESULT("DevIL", "Failed to save image. With IL error: \"" + GetILErrorString() + "\"");

		return Result();
	}
	Result Bitmap::Serialize(WriteStream& stream) const
	{
		//stream << (uint32)format << (uint32)type << Vec2<uint32>(size);		
		//
		//stream.Write(pixels, GetPixelBufferSize());
		//
		return Result();
	}
	Result Bitmap::Deserialize(ReadStream& stream)
	{		
		//Debug::LoggerListener listener;
		//
		//Clear();
		//
		//stream >> (uint32&)format >> (uint32&)type >> (Vec2<uint32>&)size;
		//
		//uint64 pixelBufferSize = GetPixelBufferSize();
		//pixels = Memory::Allocate(pixelBufferSize);
		//
		//stream.Read(pixels, pixelBufferSize);
		//
		//if (Result result = std::move(listener.GetResult()))		
		//	result.SupressFatalLogs();		
		//
		//return listener.GetResult();
		return Result();
	}	

	Result Bitmap::Create(Vec2i size, BitmapColorFormat format, BitmapColorComponentType type, const void* pixels, bool flipVertically)
	{
		//Clear();
		//
		//this->format = format;
		//this->type = type;
		//this->size = size;		
		//
		//size_t stride = size.x * BitmapColorFormatComponentCount(format) * BitmapColorComponentTypeSize(type);
		//
		//size_t bitmapSize = stride * size.y;
		//this->pixels = Memory::Allocate(bitmapSize);
		//
		//const void* src = pixels;
		//void* dst = this->pixels;
		//
		//if (src == nullptr)
		//	return Result();
		//
		//if (flipVertically)
		//{
		//	size_t srcOffset = bitmapSize - stride;
		//	size_t dstOffset = 0;
		//
		//	for (size_t i = 0; i < size.y; ++i)
		//	{
		//		memcpy((uint8*)dst + dstOffset, (uint8*)src + srcOffset, stride);
		//		srcOffset -= stride;
		//		dstOffset += stride;
		//	}
		//}
		//else
		//{
		//	memcpy(dst, src, bitmapSize);
		//}
		//
		return Result();
	}
	Bitmap& Bitmap::operator=(const Bitmap& other)
	{
		Clear();

		uint componentCount = BitmapColorFormatComponentCount(format);
		uint componentSize = BitmapColorComponentTypeSize(type);
		uint pixelSize = componentCount * componentSize;
		uint bitmapSize = pixelSize * other.size.x * other.size.y;

		pixels = Memory::Allocate(bitmapSize);
		memcpy(pixels, other.pixels, bitmapSize);

		size = other.size;
		type = other.type;
		format = other.format;

		return *this;
	}
	Bitmap& Bitmap::operator=(Bitmap&& other) noexcept
	{
		Clear();

		pixels = other.pixels;
		type = other.type;
		format = other.format;
		size = other.size;

		other.pixels = nullptr;
		other.size = Vec2i();

		return *this;
	}

		
	BitmapView::BitmapView()
		: pixels(nullptr), size(0, 0), format(BitmapColorFormat::RGBA), type(BitmapColorComponentType::Uint32)
	{
	}
	BitmapView::BitmapView(Vec2i size, BitmapColorFormat format, BitmapColorComponentType type, uint stride, const void* pixels)
		: pixels(pixels), size(size), format(format), type(type), stride(stride)
	{

	}
	BitmapView::BitmapView(const Bitmap& bm)
		: pixels(bm.GetPixels()), size(bm.GetSize()), format(bm.GetPixelFormat()), type(bm.GetPixelType()), stride(bm.GetSize().x)
	{ 
	}
	BitmapView::BitmapView(const BitmapView& bm)
		: pixels(bm.pixels), size(bm.size), format(bm.format), type(bm.type), stride(bm.stride)
	{
	}
}