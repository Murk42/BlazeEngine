#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "IL/il.h"
#include "IL/ilu.h"

#include "source/BlazeEngine/Internal/Conversions.h"

#include <codecvt>

extern std::wstring to_wstring(const std::string& s);

namespace Blaze
{
	struct ImageGuard
	{
		unsigned id;
		ImageGuard()
		{
			id = ilGenImage();
			ilBindImage(id);
		}
		~ImageGuard()
		{
			ilDeleteImage(id);
		}
	};

	Bitmap::Bitmap()
		: pixels(nullptr), format(BitmapPixelFormat::RGBA), type(BitmapPixelType::Int32), size(0, 0)
	{
	}
	Bitmap::~Bitmap()
	{
		free(pixels);		
	}

	Result Bitmap::Load(StringView path)
	{
		free(pixels);

		ImageGuard imageGuard;

		std::wstring wide = to_wstring((std::string)path.Ptr());
		if (!ilLoadImage(wide.c_str()))
		{
			format = BitmapPixelFormat::RGBA;
			type = BitmapPixelType::Int32;
			size = Vec2i(0, 0);
			pixels = nullptr;

			return BLAZE_ERROR_RESULT("DevIL", "Failed to load image with path \"" + path + "\"");
		}

		Result result;

		format = DevILToBlazePixelFormat(ilGetInteger(IL_IMAGE_FORMAT), result);
		CHECK_RESULT(result)
		type = DevILToBlazePixelType(ilGetInteger(IL_IMAGE_TYPE), result);
		CHECK_RESULT(result)

		size = Vec2i(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

		size_t stride = size.x * GetFormatDepth(format, result);
		CHECK_RESULT(result)

		size_t dataSize = stride * size.y;
		void* input = ilGetData();
		void* output = malloc(dataSize);

		if (output == nullptr)
		{
			format = BitmapPixelFormat::RGBA;
			type = BitmapPixelType::Int32;
			size = Vec2i(0, 0);
			pixels = nullptr;
			
			return BLAZE_ERROR_RESULT("Blaze Engine", "malloc failed with " + StringParsing::Convert(dataSize) + " bytes");
		}

		size_t inputOffset = dataSize - stride;
		size_t outputOffset = 0;

		for (size_t i = 0; i < size.y; ++i)
		{
			memcpy((uint8*)output + outputOffset, (uint8*)input + inputOffset, stride);
			inputOffset -= stride;
			outputOffset += stride;
		}

		this->pixels = output;

		return Result();

	}
	Result Bitmap::Save(StringView path)
	{
		ImageGuard imageGuard;
		
		Result result;

		auto formatDepth = GetFormatDepth(format, result);
		CHECK_RESULT(result);
		auto pixelFormat = DevILPixelFormat(format, result);
		CHECK_RESULT(result);
		auto pixelType = DevILPixelType(type, result);
		CHECK_RESULT(result);

		ilTexImage(size.x, size.y, 1, formatDepth, pixelFormat, pixelType, pixels);

		std::wstring wide = to_wstring((std::string)path.Ptr());

		if (!ilSaveImage(wide.c_str()))
			result += BLAZE_ERROR_LOG("DevIL", "Failed to save image with path \"" + path + "\"");

		return result;
	}

	Result Bitmap::Create(Vec2i size, BitmapPixelFormat format, BitmapPixelType type, const void* pixels)
	{
		this->size = size;
		this->format = format;
		this->type = type;		 

		Result result;
		auto pixelTypeSize = PixelTypeSize(type, result);
		CHECK_RESULT(result)
		auto formatDepth = GetFormatDepth(format, result);
		CHECK_RESULT(result)

		size_t stride = size.x * pixelTypeSize * formatDepth;
		size_t pixelsSize = size.y * stride;
		free(this->pixels);
		this->pixels = malloc(pixelsSize);

		if (this->pixels == nullptr)
		{
			this->format = BitmapPixelFormat::RGBA;
			this->type = BitmapPixelType::Int32;
			this->size = Vec2i(0, 0);			

			return BLAZE_ERROR_LOG("Blaze Engine", "malloc failed with " + StringParsing::Convert(pixelsSize) + " bytes");
		}

		if (pixels != nullptr)		
			memcpy(this->pixels, pixels, pixelsSize);		

		return result;
	}	
		
	BitmapView::BitmapView()
		: pixels(nullptr), size(0, 0), format(BitmapPixelFormat::RGBA), type(BitmapPixelType::Uint32)
	{
	}
	BitmapView::BitmapView(Vec2i size, BitmapPixelFormat format, BitmapPixelType type, void* pixels)
		: pixels(pixels), size(size), format(format), type(type)
	{

	}
	BitmapView::BitmapView(const Bitmap& bm)
		: pixels(bm.GetPixels()), size(bm.GetSize()), format(bm.GetPixelFormat()), type(bm.GetPixelType())
	{ 
	}
	BitmapView::BitmapView(const BitmapView& bm)
		: pixels(bm.pixels), size(bm.size), format(bm.format), type(bm.type)
	{
	}
}