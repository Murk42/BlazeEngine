#include "BlazeEngine/Graphics/Bitmap.h"
#include "BlazeEngine/Core/Logger.h"
#include "IL/il.h"
#include "IL/ilu.h"

namespace Blaze
{
	uint ToDevILFormat(PixelFormat format)
	{
		switch (format)
		{
		case PixelFormat::Red: return IL_LUMINANCE;
		case PixelFormat::RG: return IL_LUMINANCE_ALPHA;
		case PixelFormat::RGB: return IL_RGB;
		case PixelFormat::RGBA: return IL_RGBA;
		case PixelFormat::BGR: return IL_BGR;
		case PixelFormat::BGRA: return IL_BGRA;
		}
	}

	PixelFormat ToBlazeFormat(uint format)
	{
		switch (format)
		{
		case IL_LUMINANCE:		return PixelFormat::Red;
		case IL_LUMINANCE_ALPHA:return PixelFormat::RG;
		case IL_RGB:			return PixelFormat::RGB;
		case IL_RGBA:			return PixelFormat::RGBA;
		case IL_BGR:			return PixelFormat::BGR;
		case IL_BGRA:			return PixelFormat::BGRA;
		}
	}

	uint GetFormatDepth(PixelFormat format)
	{
		switch (format)
		{
		case PixelFormat::Red: return 1;
		case PixelFormat::RG: return 2;
		case PixelFormat::RGB: return 3;
		case PixelFormat::RGBA: return 4;
		case PixelFormat::BGR: return 3;
		case PixelFormat::BGRA: return 4;
		default:
			return 0;
		}		
	}

	Bitmap::Bitmap()		
		: format(PixelFormat::RGBA), size(0, 0), pixels(nullptr)
	{		
		id = ilGenImage();
	}

	Bitmap::~Bitmap()
	{
		ilDeleteImage(id);		
	}

	bool Bitmap::Load(const String& path, bool emitWarning)
	{						
		ilBindImage(id);		
		
		if (ilLoadImage(path.Ptr()))
		{
			iluFlipImage();
			format = ToBlazeFormat(ilGetInteger(IL_IMAGE_FORMAT));
			size = Vec2i(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
			pixels = ilGetData();
			return true;
		}

		if (emitWarning)
			Logger::AddLog(LogType::Warning, __FUNCTION__, "Failed to load image with path \"" + path + "\"");
		return false;
	}

	bool Bitmap::Save(const String& path, bool emitWarning)
	{
		ilBindImage(id);
		if (ilSaveImage(path.Ptr()))
			return true;

		if (emitWarning)
			Logger::AddLog(LogType::Warning, __FUNCTION__, "Failed to save image with path \"" + path + "\"");

		return false;
	}

	void Bitmap::Create(const Bitmap& bm)
	{
		ilBindImage(id);
		ilCopyImage(bm.id);
		size = bm.size;
		format = bm.format;
		pixels = ilGetData();
	}

	void Bitmap::Create(const Vec2i& size, PixelFormat format, void* pixels)
	{
		ilBindImage(id);
		ilTexImage(size.x, size.y, 1, GetFormatDepth(format), ToDevILFormat(format), IL_UNSIGNED_BYTE, pixels);		
		this->size = size;
		this->format = format;
		this->pixels = ilGetData();
	}

	void Bitmap::SetPixels(Vec2i offset, Vec2i size, PixelFormat format, void* pixels)
	{
		if (this->pixels == nullptr)		
			Logger::AddLog(LogType::Warning, __FUNCTION__, "Setting pixels to an empty bitmap");		
		ilBindImage(id);
		ilSetPixels(offset.x, offset.y, 0, size.x, size.y, 1, ToDevILFormat(format), IL_UNSIGNED_BYTE, pixels);
		this->pixels = ilGetData();
	}

	void Bitmap::ConvertFormat(PixelFormat format)
	{
		ilBindImage(id);
		ilConvertImage(ToDevILFormat(format), IL_UNSIGNED_INT);
		this->format = format;
		this->pixels = ilGetData();
	}
	void Bitmap::FlipVerticaly()
	{
		ilBindImage(id);
		iluFlipImage();
	}
}