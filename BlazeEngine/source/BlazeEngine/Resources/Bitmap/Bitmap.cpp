#include "pch.h"
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "sail/sail.h"
#include "SailClasses.h"

namespace Blaze
{
	template<typename T>
	inline std::underlying_type_t<T> ToInteger(T value)
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}
	uint BitmapColorFormatComponentCount(BitmapColorFormat format)
	{
		switch (format)
		{
		case Blaze::BitmapColorFormat::Red: return 1;
		case Blaze::BitmapColorFormat::RG: return 2;
		case Blaze::BitmapColorFormat::RGB:	return 3;
		case Blaze::BitmapColorFormat::RGBA: return 4;
		case Blaze::BitmapColorFormat::BGR:	return 3;
		case Blaze::BitmapColorFormat::BGRA: return 4;
		default:
			BLAZE_ENGINE_ERROR("Invalid BitmapColorFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
			return 4;
		}
	}

	uintMem BitmapColorComponentTypeSize(BitmapColorComponentType type)
	{
		switch (type)
		{
		case Blaze::BitmapColorComponentType::Int8: return sizeof(int8);
		case Blaze::BitmapColorComponentType::Uint8: return sizeof(uint8);
		case Blaze::BitmapColorComponentType::Int16:	return sizeof(int);
		case Blaze::BitmapColorComponentType::Uint16: return sizeof(uint16);
		case Blaze::BitmapColorComponentType::Int32:	return sizeof(int32);
		case Blaze::BitmapColorComponentType::Uint32: return sizeof(uint32);
		case Blaze::BitmapColorComponentType::Float:	return sizeof(float);
		case Blaze::BitmapColorComponentType::Double: return sizeof(double);
		default:
			BLAZE_ENGINE_ERROR("Invalid BitmapColorComponentType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
			return sizeof(uint8);
		}
	}

	BitmapRef::BitmapRef()
		: pixels(nullptr), format(BitmapColorFormat::RGBA), type(BitmapColorComponentType::Int32), size(0, 0), stride(0)
	{
		  
	}
	BitmapRef::BitmapRef(Vec2u size, BitmapColorFormat format, BitmapColorComponentType type, uintMem stride, void* pixels)
		: size(size), format(format), type(type), stride(stride), pixels(pixels)
	{
	}
	BitmapRef::BitmapRef(const BitmapRef& other)
		: pixels(other.pixels), format(other.format), type(other.type), size(other.size), stride(other.stride)
	{

	}

	BitmapRef BitmapRef::GetSubBitmap(Vec2u size, Vec2u offset)
	{
		uintMem pixelSize = BitmapColorFormatComponentCount(format) * BitmapColorComponentTypeSize(type);
		return BitmapRef(size, format, type, stride, (char*)pixels + offset.y * stride + offset.x * pixelSize);
	}

	Bitmap::Bitmap()
		: BitmapRef()
	{
	}
	Bitmap::Bitmap(const Bitmap& other)
	{
		uint componentCount = BitmapColorFormatComponentCount(other.format);
		uintMem componentSize = BitmapColorComponentTypeSize(other.type);
		uintMem pixelSize = componentCount * componentSize;
		uintMem bitmapSize = other.stride * other.size.y;

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
		other.size = Vec2u();
		other.stride = 0;
	}
	Bitmap::Bitmap(const BitmapView& bitmapView, uintMem newStride)
		: BitmapRef(bitmapView.GetSize(), bitmapView.GetPixelFormat(), bitmapView.GetPixelType(), newStride, nullptr)
	{		
		uintMem pixelSize = BitmapColorFormatComponentCount(format) * BitmapColorComponentTypeSize(type);
		uintMem minimalStride = pixelSize * size.x;
		uintMem bitmapSize = stride * size.y;
		pixels = Memory::Allocate(bitmapSize);		
		
		const void* src = bitmapView.GetPixels();
		void* dst = pixels;

		for (uintMem y = 0; y < size.y; ++y)
		{
			memcpy(dst, src, minimalStride);

			src = (char*)src + bitmapView.GetStride();
			dst = (char*)dst + stride;
		} 
	}
	Bitmap::~Bitmap()
	{
		Clear();
	}

	void Bitmap::Clear()
	{
		Memory::Free(pixels);
		pixels = nullptr;
		size = Vec2u();
		stride = 0;
	}

	bool ConvertToConvenientImage(sail_image* image, BitmapColorFormat& format, BitmapColorComponentType& type)
	{
		auto pixelFormat = image->pixel_format;
		switch (pixelFormat)
		{
		case SAIL_PIXEL_FORMAT_UNKNOWN: return false;			
		case SAIL_PIXEL_FORMAT_BPP1: return false;
		case SAIL_PIXEL_FORMAT_BPP2: return false;
		case SAIL_PIXEL_FORMAT_BPP4: return false;
		case SAIL_PIXEL_FORMAT_BPP8: format = BitmapColorFormat::Red; type = BitmapColorComponentType::Uint8; return true;
		case SAIL_PIXEL_FORMAT_BPP16: format = BitmapColorFormat::RG; type = BitmapColorComponentType::Uint8; return true;			
		case SAIL_PIXEL_FORMAT_BPP24: format = BitmapColorFormat::RGB; type = BitmapColorComponentType::Uint8; return true;
		case SAIL_PIXEL_FORMAT_BPP32: format = BitmapColorFormat::RGBA; type = BitmapColorComponentType::Uint8; return true;
		case SAIL_PIXEL_FORMAT_BPP48: format = BitmapColorFormat::RGB; type = BitmapColorComponentType::Uint16; return true;
		case SAIL_PIXEL_FORMAT_BPP64: format = BitmapColorFormat::RGBA; type = BitmapColorComponentType::Uint16; return true;
		case SAIL_PIXEL_FORMAT_BPP72: return false;
		case SAIL_PIXEL_FORMAT_BPP96: format = BitmapColorFormat::RGB; type = BitmapColorComponentType::Uint32; return true;
		case SAIL_PIXEL_FORMAT_BPP128: format = BitmapColorFormat::RGBA; type = BitmapColorComponentType::Uint32; return true;
		case SAIL_PIXEL_FORMAT_BPP1_INDEXED: return false;
		case SAIL_PIXEL_FORMAT_BPP2_INDEXED: return false;
		case SAIL_PIXEL_FORMAT_BPP4_INDEXED: return false;
		case SAIL_PIXEL_FORMAT_BPP8_INDEXED: return false;
		case SAIL_PIXEL_FORMAT_BPP16_INDEXED: return false;
		case SAIL_PIXEL_FORMAT_BPP1_GRAYSCALE: return false;
		case SAIL_PIXEL_FORMAT_BPP2_GRAYSCALE: return false;
		case SAIL_PIXEL_FORMAT_BPP4_GRAYSCALE: return false;
		case SAIL_PIXEL_FORMAT_BPP8_GRAYSCALE: format = BitmapColorFormat::Red; type = BitmapColorComponentType::Uint8; return true;
		case SAIL_PIXEL_FORMAT_BPP16_GRAYSCALE: format = BitmapColorFormat::Red; type = BitmapColorComponentType::Uint16; return true;
		case SAIL_PIXEL_FORMAT_BPP4_GRAYSCALE_ALPHA: return false;
		case SAIL_PIXEL_FORMAT_BPP8_GRAYSCALE_ALPHA: return false;
		case SAIL_PIXEL_FORMAT_BPP16_GRAYSCALE_ALPHA: return false;
		case SAIL_PIXEL_FORMAT_BPP32_GRAYSCALE_ALPHA: return false;
		case SAIL_PIXEL_FORMAT_BPP16_RGB555: return false;
		case SAIL_PIXEL_FORMAT_BPP16_BGR555: return false;
		case SAIL_PIXEL_FORMAT_BPP16_RGB565: return false;
		case SAIL_PIXEL_FORMAT_BPP16_BGR565: return false;
		case SAIL_PIXEL_FORMAT_BPP24_RGB: format = BitmapColorFormat::RGB; type = BitmapColorComponentType::Uint8; return true;
		case SAIL_PIXEL_FORMAT_BPP24_BGR:format = BitmapColorFormat::BGR; type = BitmapColorComponentType::Uint8; return true;
		case SAIL_PIXEL_FORMAT_BPP48_RGB:format = BitmapColorFormat::RGB; type = BitmapColorComponentType::Uint16; return true;
		case SAIL_PIXEL_FORMAT_BPP48_BGR:format = BitmapColorFormat::BGR; type = BitmapColorComponentType::Uint16; return true;
		case SAIL_PIXEL_FORMAT_BPP16_RGBX: return false;
		case SAIL_PIXEL_FORMAT_BPP16_BGRX: return false;
		case SAIL_PIXEL_FORMAT_BPP16_XRGB: return false;
		case SAIL_PIXEL_FORMAT_BPP16_XBGR: return false;
		case SAIL_PIXEL_FORMAT_BPP16_RGBA: return false;
		case SAIL_PIXEL_FORMAT_BPP16_BGRA: return false;
		case SAIL_PIXEL_FORMAT_BPP16_ARGB: return false;
		case SAIL_PIXEL_FORMAT_BPP16_ABGR: return false;
		case SAIL_PIXEL_FORMAT_BPP32_RGBX: return false;
		case SAIL_PIXEL_FORMAT_BPP32_BGRX: return false;
		case SAIL_PIXEL_FORMAT_BPP32_XRGB: return false;
		case SAIL_PIXEL_FORMAT_BPP32_XBGR: return false;
		case SAIL_PIXEL_FORMAT_BPP32_RGBA: format = BitmapColorFormat::RGBA; type = BitmapColorComponentType::Uint8; return true;
		case SAIL_PIXEL_FORMAT_BPP32_BGRA: format = BitmapColorFormat::BGRA; type = BitmapColorComponentType::Uint8; return true;
		case SAIL_PIXEL_FORMAT_BPP32_ARGB: return false;
		case SAIL_PIXEL_FORMAT_BPP32_ABGR: return false;
		case SAIL_PIXEL_FORMAT_BPP64_RGBX: return false;
		case SAIL_PIXEL_FORMAT_BPP64_BGRX: return false;
		case SAIL_PIXEL_FORMAT_BPP64_XRGB: return false;
		case SAIL_PIXEL_FORMAT_BPP64_XBGR: return false;
		case SAIL_PIXEL_FORMAT_BPP64_RGBA: return false;
		case SAIL_PIXEL_FORMAT_BPP64_BGRA: return false;
		case SAIL_PIXEL_FORMAT_BPP64_ARGB: return false;
		case SAIL_PIXEL_FORMAT_BPP64_ABGR: return false;
		case SAIL_PIXEL_FORMAT_BPP32_CMYK: return false;
		case SAIL_PIXEL_FORMAT_BPP64_CMYK: return false;
		case SAIL_PIXEL_FORMAT_BPP40_CMYKA: return false;
		case SAIL_PIXEL_FORMAT_BPP80_CMYKA: return false;
		case SAIL_PIXEL_FORMAT_BPP24_YCBCR: return false;
		case SAIL_PIXEL_FORMAT_BPP32_YCCK: return false;
		case SAIL_PIXEL_FORMAT_BPP24_CIE_LAB: return false;
		case SAIL_PIXEL_FORMAT_BPP40_CIE_LAB: return false;
		case SAIL_PIXEL_FORMAT_BPP24_CIE_LUV: return false;
		case SAIL_PIXEL_FORMAT_BPP40_CIE_LUV: return false;
		case SAIL_PIXEL_FORMAT_BPP24_YUV: return false;
		case SAIL_PIXEL_FORMAT_BPP30_YUV: return false;
		case SAIL_PIXEL_FORMAT_BPP36_YUV: return false;
		case SAIL_PIXEL_FORMAT_BPP48_YUV: return false;
		case SAIL_PIXEL_FORMAT_BPP32_YUVA: return false;			
		case SAIL_PIXEL_FORMAT_BPP40_YUVA: return false;			
		case SAIL_PIXEL_FORMAT_BPP48_YUVA: return false;			
		case SAIL_PIXEL_FORMAT_BPP64_YUVA: return false;
		default: return false;
		}		
	}	

	bool GetSailPixelFormat(BitmapColorFormat format, BitmapColorComponentType type, SailPixelFormat& outFormat)
	{
		switch (format)
		{
		case Blaze::BitmapColorFormat::Red:
			switch (type)
			{
			case Blaze::BitmapColorComponentType::Int8: return false;				
			case Blaze::BitmapColorComponentType::Uint8:outFormat = SAIL_PIXEL_FORMAT_BPP8_GRAYSCALE; return true;
			case Blaze::BitmapColorComponentType::Int16: return false;				
			case Blaze::BitmapColorComponentType::Uint16:outFormat = SAIL_PIXEL_FORMAT_BPP16_GRAYSCALE; return true;
			case Blaze::BitmapColorComponentType::Int32: return false;
			case Blaze::BitmapColorComponentType::Uint32: return false;
			case Blaze::BitmapColorComponentType::Float: return false;
			case Blaze::BitmapColorComponentType::Double: return false;
			default: return false;
			}			
		case Blaze::BitmapColorFormat::RG:
			switch (type)
			{
			case Blaze::BitmapColorComponentType::Int8: return false;
			case Blaze::BitmapColorComponentType::Uint8:outFormat = SAIL_PIXEL_FORMAT_BPP16_GRAYSCALE; return true;
			case Blaze::BitmapColorComponentType::Int16: return false;
			case Blaze::BitmapColorComponentType::Uint16: return false;
			case Blaze::BitmapColorComponentType::Int32: return false;
			case Blaze::BitmapColorComponentType::Uint32: return false;
			case Blaze::BitmapColorComponentType::Float: return false;
			case Blaze::BitmapColorComponentType::Double: return false;
			default: return false;
			}			
		case Blaze::BitmapColorFormat::RGB:
			switch (type)
			{
			case Blaze::BitmapColorComponentType::Int8: return false;
			case Blaze::BitmapColorComponentType::Uint8:outFormat = SAIL_PIXEL_FORMAT_BPP24_RGB; return true;
			case Blaze::BitmapColorComponentType::Int16: return false;
			case Blaze::BitmapColorComponentType::Uint16:outFormat = SAIL_PIXEL_FORMAT_BPP48_RGB; return true;
			case Blaze::BitmapColorComponentType::Int32: return false;
			case Blaze::BitmapColorComponentType::Uint32:outFormat = SAIL_PIXEL_FORMAT_BPP96; return true;
			case Blaze::BitmapColorComponentType::Float: return false;
			case Blaze::BitmapColorComponentType::Double: return false;
			default: return false;
			}			
		case Blaze::BitmapColorFormat::RGBA:
			switch (type)
			{
			case Blaze::BitmapColorComponentType::Int8: return false;
			case Blaze::BitmapColorComponentType::Uint8:outFormat = SAIL_PIXEL_FORMAT_BPP32_RGBA; return true;
			case Blaze::BitmapColorComponentType::Int16: return false;
			case Blaze::BitmapColorComponentType::Uint16:outFormat = SAIL_PIXEL_FORMAT_BPP64_RGBA; return true;
			case Blaze::BitmapColorComponentType::Int32: return false;
			case Blaze::BitmapColorComponentType::Uint32:outFormat = SAIL_PIXEL_FORMAT_BPP128; return true;
			case Blaze::BitmapColorComponentType::Float: return false;
			case Blaze::BitmapColorComponentType::Double: return false;
			default: return false;
			}			
		case Blaze::BitmapColorFormat::BGR:
			switch (type)
			{
			case Blaze::BitmapColorComponentType::Int8: return false;
			case Blaze::BitmapColorComponentType::Uint8:outFormat = SAIL_PIXEL_FORMAT_BPP24_BGR; return true;
			case Blaze::BitmapColorComponentType::Int16: return false;
			case Blaze::BitmapColorComponentType::Uint16:outFormat = SAIL_PIXEL_FORMAT_BPP48_BGR; return true;
			case Blaze::BitmapColorComponentType::Int32: return false;
			case Blaze::BitmapColorComponentType::Uint32: return false;
			case Blaze::BitmapColorComponentType::Float: return false;
			case Blaze::BitmapColorComponentType::Double: return false;
			default: return false;
			}			
		case Blaze::BitmapColorFormat::BGRA:
			switch (type)
			{
			case Blaze::BitmapColorComponentType::Int8: return false;
			case Blaze::BitmapColorComponentType::Uint8:outFormat = SAIL_PIXEL_FORMAT_BPP32_BGRA; return true;
			case Blaze::BitmapColorComponentType::Int16: return false;
			case Blaze::BitmapColorComponentType::Uint16:outFormat = SAIL_PIXEL_FORMAT_BPP64_BGRA; return true;
			case Blaze::BitmapColorComponentType::Int32: return false;
			case Blaze::BitmapColorComponentType::Uint32: return false;
			case Blaze::BitmapColorComponentType::Float: return false;
			case Blaze::BitmapColorComponentType::Double: return false;
			default: return false;
			}
		default: return false;
		}  
	}

	void Bitmap::Load(Path path, bool flipVertically)
	{
		const sail_codec_info* codec = nullptr;
		sail_image* image;
		void* state = nullptr;

		StringUTF8 pathString = path.ToString();
		StringUTF8 fileExtension = path.FileExtension();
		StringParsing::GetAfterFirst(fileExtension, fileExtension, '.');

		SAIL_CHECK(sail_codec_info_from_extension((const char*)fileExtension.Buffer(), &codec), "Failed to get codec info object from \"" + fileExtension + "\" extension.");
		SAIL_CHECK(sail_start_loading_from_file((const char*)pathString.Buffer(), codec, &state), "Failed to start loading from file with path \"" + pathString + "\".");
		SAIL_CHECK(sail_load_next_frame(state, &image), "Failed to load next from file with path \"" + pathString + "\".");

		BitmapColorFormat format{ };
		BitmapColorComponentType type{ };
		Vec2u size = Vec2u(image->width, image->height);

		if (!ConvertToConvenientImage(image, format, type))
		{
			BLAZE_ENGINE_ERROR("Unsupported color format for deserialization");
			return;
		}

		Create(size, format, type, image->pixels, image->bytes_per_line, flipVertically);

		SAIL_CHECK(sail_stop_loading(state), "Failed to stop loading.");		
		sail_destroy_image(image);		
	}	
	void Bitmap::Save(Path path)
	{				
		const sail_codec_info* codec = nullptr;
		sail_image image;
		void* state = nullptr;

		SailPixelFormat pixel_format;
		GetSailPixelFormat(format, type, pixel_format);

		image.pixels = pixels;
		image.width = size.x;
		image.height = size.y;
		image.bytes_per_line = stride;
		image.resolution = NULL;
		image.pixel_format = pixel_format;
		image.gamma = 1;
		image.delay = 0;
		image.palette = NULL;
		image.meta_data_node = NULL;
		image.iccp = NULL;
		
		StringUTF8 pathString = path.ToString();
		StringUTF8 fileExtension = path.FileExtension();
		StringParsing::GetAfterFirst(fileExtension, fileExtension, '.');

		SAIL_CHECK(sail_codec_info_from_extension((const char*)fileExtension.Buffer(), &codec), "Failed to get codec info object from \"" + fileExtension + "\" extension.");
		SAIL_CHECK(sail_start_saving_into_file((const char*)pathString.Buffer(), codec, &state), "Failed to start saving into from file with path \"" + pathString + "\".");
		SAIL_CHECK(sail_write_next_frame(state, &image), "Failed to load next frame from file with path \"" + pathString + "\".");

		SAIL_CHECK(sail_stop_saving(state), "Failed to stop saving.");		
	}
	void Bitmap::Serialize(WriteStream& stream) const
	{
		sail_io* io = nullptr;
		const sail_codec_info* codec = nullptr;
		sail_image image;
		void* state = nullptr;

		SailPixelFormat pixel_format;
		GetSailPixelFormat(format, type, pixel_format);

		image.pixels = pixels;
		image.width = size.x;
		image.height = size.y;
		image.bytes_per_line = stride;
		image.resolution = NULL;
		image.pixel_format = pixel_format;
		image.gamma = 1;
		image.delay = 0;
		image.palette = NULL;
		image.meta_data_node = NULL;
		image.iccp = NULL;
				
		SAIL_CHECK(sail_alloc_io(&io), "Failed to allocate io object.");
		SetupSailWriteIO(io, stream);
		SAIL_CHECK(sail_codec_info_from_extension("bmp", &codec), "Failed to get codec info object from \".bmp\" extension.");
		SAIL_CHECK(sail_start_saving_into_io(io, codec, &state), "Failed to start saving into io.");
		SAIL_CHECK(sail_write_next_frame(state, &image), "Failed to load next frame.");		
		
		SAIL_CHECK(sail_stop_saving(state), "Failed to stop saving");
		sail_destroy_io(io);				
	}
	void Bitmap::Deserialize(ReadStream& stream)
	{	
		sail_io* io = nullptr;		
		const sail_codec_info* codec = nullptr;
		sail_image* image;
		void* state = nullptr;
		
		SAIL_CHECK(sail_alloc_io(&io), "Failed to allocate io object.");
		SetupSailReadIO(io, stream);
		SAIL_CHECK(sail_codec_info_from_extension("bmp", &codec), "Failed to get codec info object from \".bmp\" extension.");
		SAIL_CHECK(sail_start_loading_from_io(io, codec, &state), "Failed to start loading form io.");
		SAIL_CHECK(sail_load_next_frame(state, &image), "Failed to load next frame.");
		
		BitmapColorFormat format{ };
		BitmapColorComponentType type{ };
		Vec2u size = Vec2u(image->width, image->height);

		if (!ConvertToConvenientImage(image, format, type))
		{
			BLAZE_ENGINE_ERROR("Unsupported color format for deserialization");
			return;
		}

		Create(size, format, type, image->pixels, image->bytes_per_line, false);

		SAIL_CHECK(sail_stop_loading(state), "Failed to stop loading");
		sail_destroy_io(io);
		sail_destroy_image(image);		
	}
	void Bitmap::Create(Vec2u size, BitmapColorFormat format, BitmapColorComponentType type, const void* pixels, uintMem stride, bool flipVertically)
	{
		Clear();

		if (stride == 0)
		{
			uint componentCount = BitmapColorFormatComponentCount(format);
			uintMem componentSize = BitmapColorComponentTypeSize(type);
			uintMem pixelSize = componentCount * componentSize;

			stride = pixelSize * size.x;
		}
		
		this->format = format;
		this->type = type;
		this->size = size;						
		this->stride = stride;
		
		uintMem bitmapSize = stride * size.y;
		this->pixels = Memory::Allocate(bitmapSize);
		
		const void* src = pixels;
		void* dst = this->pixels;
		
		if (src == nullptr)
			return;
		
		if (flipVertically)
		{
			uintMem srcOffset = bitmapSize - stride;
			uintMem dstOffset = 0;
		
			for (uintMem i = 0; i < size.y; ++i)
			{
				memcpy((uint8*)dst + dstOffset, (uint8*)src + srcOffset, stride);
				srcOffset -= stride;
				dstOffset += stride;
			}
		}
		else
		{
			memcpy(dst, src, bitmapSize);
		}		
	}	

	Bitmap& Bitmap::operator=(const Bitmap& other)
	{
		Clear();

		uint componentCount = BitmapColorFormatComponentCount(format);
		uintMem componentSize = BitmapColorComponentTypeSize(type);
		uintMem pixelSize = componentCount * componentSize;
		uintMem bitmapSize = pixelSize * other.size.x * other.size.y;

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
		other.size = Vec2u();

		return *this;
	}


	BitmapView::BitmapView()
		: pixels(nullptr), size(0, 0), format(BitmapColorFormat::RGBA), type(BitmapColorComponentType::Uint32)
	{
	}
	BitmapView::BitmapView(Vec2u size, BitmapColorFormat format, BitmapColorComponentType type, uintMem stride, const void* pixels)
		: pixels(pixels), size(size), format(format), type(type), stride(stride)
	{

	}
	BitmapView::BitmapView(const Bitmap& bm)
		: pixels(bm.GetPixels()), size(bm.GetSize()), format(bm.GetPixelFormat()), type(bm.GetPixelType()), stride(bm.GetStride())
	{
	}
	BitmapView::BitmapView(const BitmapRef& bm)
		: pixels(bm.GetPixels()), size(bm.GetSize()), format(bm.GetPixelFormat()), type(bm.GetPixelType()), stride(bm.GetStride())
	{
	}
	BitmapView::BitmapView(const BitmapView& bm)
		: pixels(bm.pixels), size(bm.size), format(bm.format), type(bm.type), stride(bm.stride)
	{
	}

	BitmapView BitmapView::GetSubBitmap(Vec2u size, Vec2u offset)
	{
		uintMem pixelSize = BitmapColorFormatComponentCount(format) * BitmapColorComponentTypeSize(type);
		return BitmapRef(size, format, type, stride, (char*)pixels + offset.y * stride + offset.x * pixelSize);
	}
}