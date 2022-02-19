#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Logger/Logger.h"
#include "IL/il.h"
#include "IL/ilu.h"

#include "source/BlazeEngine/Internal/Conversions.h"


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

	bool Bitmap::Load(StringView path, bool emitWarning)
	{
		ImageGuard imageGuard;

		if (ilLoadImage(path.Ptr()))
		{
			format = DevILToBlazePixelFormat(ilGetInteger(IL_IMAGE_FORMAT));
			type = DevILToBlazePixelType(ilGetInteger(IL_IMAGE_TYPE));
			size = Vec2i(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
			
			size_t stride = size.x * GetFormatDepth(format);
			size_t dataSize = stride * size.y;
			void* input = ilGetData();
			void* output = malloc(dataSize);
	
			size_t inputOffset = dataSize - stride;
			size_t outputOffset = 0;
			for (size_t i = 0; i < size.y; ++i)
			{
				memcpy((uint8*)output + outputOffset, (uint8*)input + inputOffset, stride);
				inputOffset -= stride;
				outputOffset += stride;
			}		

			this->pixels = output;

			return true;
		}

		if (emitWarning)
			BLAZE_WARNING_LOG("DevIL", "Failed to load image with path \"" + path + "\"");
		return false;
	}
	bool Bitmap::Save(StringView path, bool emitWarning)
	{
		ImageGuard imageGuard;
		
		ilTexImage(size.x, size.y, 1, GetFormatDepth(format), DevILPixelFormat(format), DevILPixelType(type), pixels);
		if (ilSaveImage(path.Ptr()))
			return BLAZE_OK;

		if (emitWarning)
			BLAZE_WARNING_LOG("DevIL", "Failed to save image with path \"" + path + "\"");

		return BLAZE_ERROR;
	}

	void Bitmap::Create(Vec2i size, BitmapPixelFormat format, BitmapPixelType type, void* pixels)
	{
		this->size = size;
		this->format = format;
		this->type = type;		

		size_t stride = size.x * PixelTypeSize(type);
		size_t pixelsSize = size.y * stride;
		free(this->pixels);
		this->pixels = malloc(pixelsSize);

		if (pixels != nullptr)		
			memcpy(this->pixels, pixels, pixelsSize);		
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