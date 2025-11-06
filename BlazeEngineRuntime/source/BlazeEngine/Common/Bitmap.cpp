#include "pch.h"
#include "BlazeEngine/Core/Memory/MemoryManager.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/Core/Common/Stream.h"
#include "BlazeEngine/Common/Bitmap.h"
#include "BlazeEngine/External/sail/sail.h"

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
			BLAZE_LOG_ERROR("Invalid BitmapColorFormat enum value. The integer value was: {}", ToInteger(format));
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
			BLAZE_LOG_ERROR("Invalid BitmapColorComponentType enum value. The integer value was: {}", ToInteger(type));
			return sizeof(uint8);
		}
	}

	BitmapRef::BitmapRef()
		: pixels(nullptr), format(BitmapColorFormat::RGBA), type(BitmapColorComponentType::Uint8), size(0, 0), stride(0)
	{
	}
	BitmapRef::BitmapRef(Vec2u size, BitmapColorFormat format, BitmapColorComponentType type, uintMem stride, void* pixels)
		: size(size), format(format), type(type), stride(stride), pixels(pixels)
	{
		if (size.x == 0 || size.y == 0 || pixels == nullptr)
		{
			size = Vec2u();
			pixels = nullptr;
			format = BitmapColorFormat::RGBA;
			type = BitmapColorComponentType::Uint8;
			stride = 0;
		}
	}
	BitmapRef::BitmapRef(const BitmapRef& other)
		: pixels(other.pixels), format(other.format), type(other.type), size(other.size), stride(other.stride)
	{

	}
	BitmapRef BitmapRef::SubBitmap(Vec2u size, Vec2u offset)
	{
		uintMem pixelSize = BitmapColorFormatComponentCount(format) * BitmapColorComponentTypeSize(type);
		return BitmapRef(size, format, type, stride, (char*)pixels + offset.y * stride + offset.x * pixelSize);
	}
	BitmapRef& BitmapRef::operator=(const BitmapRef& other)
	{
		pixels = other.pixels;
		format = other.format;
		type = other.type;
		size = other.size;
		stride = other.stride;
		return *this;
	}

	Bitmap::Bitmap()
	{
	}
	Bitmap::Bitmap(const Bitmap& other)
	{
		if (other.Empty())
			return;

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
		: BitmapRef(std::move(other))
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

	static bool ConvertToConvenientImage(const sail_image* image, BitmapColorFormat& format, BitmapColorComponentType& type)
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
	static bool GetSailPixelFormat(BitmapColorFormat format, BitmapColorComponentType type, SailPixelFormat& outFormat)
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
	static bool CopySailImageToBitmap(const sail_image* image, Bitmap& bitmap, bool flipVertically)
	{
		BitmapColorFormat format{ };
		BitmapColorComponentType type{ };
		Vec2u size = Vec2u(image->width, image->height);

		if (!ConvertToConvenientImage(image, format, type))
		{
			BLAZE_LOG_ERROR("Unsupported SAIL pixel format");
			return false;
		}

		bitmap.Create(size, format, type, image->pixels, image->bytes_per_line, flipVertically);

		return true;
	}
	static bool MakeSailImageViewIntoBitmap(const BitmapView& bitmap, sail_image& image)
	{
		SailPixelFormat pixel_format;

		if (!GetSailPixelFormat(bitmap.GetPixelFormat(), bitmap.GetPixelType(), pixel_format))
			return false;

		image.pixels = (void*)bitmap.GetPixels();
		image.width = bitmap.GetSize().x;
		image.height = bitmap.GetSize().y;
		image.bytes_per_line = static_cast<uint>(bitmap.GetStride());
		image.resolution = NULL;
		image.pixel_format = pixel_format;
		image.gamma = 1;
		image.delay = 0;
		image.palette = NULL;
		image.meta_data_node = NULL;
		image.iccp = NULL;

		return true;
	}

	bool Bitmap::Load(Path path, bool flipVertically)
	{
		sail_image* image;
		void* state = nullptr;

		SAIL_CHECK_RET(sail_start_loading_from_file(reinterpret_cast<const char*>(path.GetString().Ptr()), NULL, &state), false, "sail_start_loading_from_file failed with path \"{}\"", path);
		SAIL_CHECK_RET(sail_load_next_frame(state, &image), false, "sail_load_next_frame failed.");

		if (!CopySailImageToBitmap(image, *this, flipVertically))
			return false;

		SAIL_CHECK_RET(sail_stop_loading(state), false, "sail_stop_loading failed.");
		sail_destroy_image(image);

		return true;
	}
	bool Bitmap::Load(ReadStream& stream, bool flipVertically)
	{
		sail_io* io = nullptr;
		const sail_codec_info* codec = nullptr;
		sail_image* image;
		void* state = nullptr;

		stream.SetPosition(0);

		SAIL_CHECK_RET(sail_alloc_io(&io), false, "sail_alloc_io failed");
		sail_SetupReadIO(io, stream);

		SAIL_CHECK_RET(sail_codec_info_by_magic_number_from_io(io, &codec), false, "sail_codec_info_by_magic_number_from_io failed");
		SAIL_CHECK_RET(sail_start_loading_from_io(io, codec, &state), false, "sail_start_loading_from_io failed");
		SAIL_CHECK_RET(sail_load_next_frame(state, &image), false, "sail_load_next_frame failed");

		if (!CopySailImageToBitmap(image, *this, flipVertically))
			return false;

		SAIL_CHECK_RET(sail_stop_loading(state), false, "sail_stop_loading failed");
		sail_destroy_image(image);

		sail_destroy_io(io);

		return true;
	}
	bool Bitmap::LoadWithMIME(ReadStream& stream, u8String imageMIMEType, bool flipVertically)
	{
		sail_io* io = nullptr;
		const sail_codec_info* codec = nullptr;
		sail_image* image;
		void* state = nullptr;

		stream.SetPosition(0);

		SAIL_CHECK_RET(sail_alloc_io(&io), false, "sail_alloc_io failed");
		sail_SetupReadIO(io, stream);

		if (SAIL_NO_LOGS(sail_codec_info_from_mime_type(reinterpret_cast<const char*>(imageMIMEType.Ptr()), &codec)) != SAIL_OK)
			SAIL_CHECK_RET(sail_codec_info_by_magic_number_from_io(io, &codec), false, "Failed to find SAIL codec from MIME \"{}\"", imageMIMEType);

		SAIL_CHECK_RET(sail_start_loading_from_io(io, codec, &state), false, "sail_start_loading_from_io failed");
		SAIL_CHECK_RET(sail_load_next_frame(state, &image), false, "sail_load_next_frame failed");

		if (!CopySailImageToBitmap(image, *this, flipVertically))
			return false;

		SAIL_CHECK_RET(sail_stop_loading(state), false, "sail_stop_loading failed");
		sail_destroy_image(image);

		sail_destroy_io(io);

		return true;
	}
	bool Bitmap::LoadWithExtension(ReadStream& stream, u8String extension, bool flipVertically)
	{
		sail_io* io = nullptr;
		const sail_codec_info* codec = nullptr;
		sail_image* image;
		void* state = nullptr;

		stream.SetPosition(0);

		SAIL_CHECK_RET(sail_alloc_io(&io), false, "sail_alloc_io failed");
		sail_SetupReadIO(io, stream);

		if (SAIL_NO_LOGS(sail_codec_info_from_extension(reinterpret_cast<const char*>(extension.Ptr()), &codec)) != SAIL_OK)
			SAIL_CHECK_RET(sail_codec_info_by_magic_number_from_io(io, &codec), false, "Failed to find SAIL codec from extension \"{}\"", extension);

		SAIL_CHECK_RET(sail_start_loading_from_io(io, codec, &state), false, "sail_start_loading_from_io failed");
		SAIL_CHECK_RET(sail_load_next_frame(state, &image), false, "sail_load_next_frame failed");

		if (!CopySailImageToBitmap(image, *this, flipVertically))
			return false;

		SAIL_CHECK_RET(sail_stop_loading(state), false, "sail_stop_loading failed");
		sail_destroy_image(image);

		sail_destroy_io(io);

		return true;
	}
	bool Bitmap::Save(Path path)
	{
		sail_image image;
		void* state = nullptr;

		if (!MakeSailImageViewIntoBitmap(*this, image))
			return false;

		SAIL_CHECK_RET(sail_start_saving_into_file(reinterpret_cast<const char*>(path.GetString().Ptr()), NULL, &state), false, "sail_start_saving_into_file failed with path \"{}\"", path);
		SAIL_CHECK_RET(sail_write_next_frame(state, &image), false, "sail_write_next_frame failed");

		SAIL_CHECK_RET(sail_stop_saving(state), false, "sail_stop_saving failed");

		return true;
	}
	//It is important that imageType is StirngUTF8 and not u8StringView because it must be null-terminated
	bool Bitmap::Save(WriteStream& stream, u8String imageType)
	{
		sail_io* io = nullptr;
		const sail_codec_info* codec = nullptr;
		sail_image image;
		void* state = nullptr;

		stream.SetPosition(0);

		if (!MakeSailImageViewIntoBitmap(*this, image))
			return false;

		SAIL_CHECK_RET(sail_alloc_io(&io), false, "sail_alloc_io failed");
		sail_SetupWriteIO(io, stream);

		SAIL_CHECK_RET(sail_codec_info_from_extension(reinterpret_cast<const char*>(imageType.Ptr()), &codec), false, "sail_codec_info_from_extension failed");
		SAIL_CHECK_RET(sail_start_saving_into_io(io, codec, &state), false, "sail_start_saving_into_io failed");
		SAIL_CHECK_RET(sail_write_next_frame(state, &image), false, "sail_write_next_frame failed");

		SAIL_CHECK_RET(sail_stop_saving(state), false, "sail_stop_saving failed");
		sail_destroy_io(io);

		return true;
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
	BitmapRef Bitmap::SubBitmap(Vec2u size, Vec2u offset)
	{
		uintMem pixelSize = BitmapColorFormatComponentCount(format) * BitmapColorComponentTypeSize(type);
		return BitmapRef(size, format, type, stride, (char*)pixels + offset.y * stride + offset.x * pixelSize);
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

		BitmapRef::operator=(other);

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
	BitmapView BitmapView::SubBitmap(Vec2u size, Vec2u offset)
	{
		uintMem pixelSize = BitmapColorFormatComponentCount(format) * BitmapColorComponentTypeSize(type);
		return BitmapRef(size, format, type, stride, (char*)pixels + offset.y * stride + offset.x * pixelSize);
	}
}